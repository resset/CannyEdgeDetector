/**
 * \file      CannyEdgeDetector.cpp
 * \brief     Canny algorithm class file.
 * \details   This file is part of student project. Some parts of code may be
 *            influenced by various examples found on internet.
 * \author    resset <silentdemon@gmail.com>
 * \date      2006-2012
 * \copyright GNU General Public License, http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include <math.h>

#include "CannyEdgeDetector.h"

CannyEdgeDetector::CannyEdgeDetector()
{
	width = (unsigned int) 0;
	height = (unsigned int) 0;
	x = (unsigned int) 0;
	y = (unsigned int) 0;
	mask_halfsize = (unsigned int) 0;
}

CannyEdgeDetector::~CannyEdgeDetector()
{
	delete[] edge_magnitude;
	delete[] edge_direction;
	delete[] workspace_bitmap;
}

uint8_t* CannyEdgeDetector::ProcessImage(uint8_t* source_bitmap, unsigned int width,
                                         unsigned int height, float sigma,
                                         uint8_t lowThreshold, uint8_t highThreshold)
{
	/*
	 * Setting up image width and height in pixels.
	 */
	this->width = width;
	this->height = height;

	/*
	 * We store image in array of bytes (chars) in BGR(BGRBGRBGR...) order.
	 * Size of the table is width * height * 3 bytes.
	 */
	this->source_bitmap = source_bitmap;

	/*
	 * Conversion to grayscale. Only luminance information remains.
	 */
	this->Luminance();

	/*
	 * "Widening" image. At this step we already need to know the size of
	 * gaussian mask.
	 */
	this->PreProcessImage(sigma);

	/*
	 * Noise reduction - Gaussian filter.
	 */
	this->GaussianBlur(sigma);

	/*
	 * Edge detection - Sobel filter.
	 */
	this->EdgeDetection();

	/*
	 * Suppression of non maximum pixels.
	 */
	this->NonMaxSuppression();

	/*
	 * Hysteresis thresholding.
	 */
	this->Hysteresis(lowThreshold, highThreshold);

	/*
	 * "Shrinking" image.
	 */
	this->PostProcessImage();

	return source_bitmap;
}

inline uint8_t CannyEdgeDetector::GetPixelValue(unsigned int x, unsigned int y)
{
	return (uint8_t) *(workspace_bitmap + (unsigned long) (x * width + y));
}

inline void CannyEdgeDetector::SetPixelValue(unsigned int x, unsigned int y,
                                             uint8_t value)
{
	workspace_bitmap[(unsigned long) (x * width + y)] = value;
}

void CannyEdgeDetector::PreProcessImage(float sigma)
{
	// Finding mask size with given sigma.
	mask_size = 2 * round(sqrt(-log(0.3) * 2 * sigma * sigma)) + 1;
	mask_halfsize = mask_size / 2;

	// Enlarging workspace bitmap width and height.
	height += mask_halfsize * 2;
	width += mask_halfsize * 2;
	// Working area.
	workspace_bitmap = new uint8_t[height * width];

	// Edge information arrays.
	edge_magnitude = new float[width * height];
	edge_direction = new uint8_t[width * height];

	// Zeroing direction array.
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			edge_direction[x * width + y] = 0;
		}
	}

	// Copying image data into work area.
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			// Upper left corner.
			if (x < mask_halfsize &&  y < mask_halfsize) {
				SetPixelValue(x, y, *(source_bitmap));
			}
			// Bottom left corner.
			else if (x >= height - mask_halfsize && y < mask_halfsize) {
				SetPixelValue(x, y, *(source_bitmap + (height - 2 * mask_halfsize - 1) * 3 * (width - 2 * mask_halfsize)));
			}
			// Upper right corner.
			else if (x < mask_halfsize && y >= width - mask_halfsize) {
				SetPixelValue(x, y, *(source_bitmap + 3 * (width - 2 * mask_halfsize - 1)));
			}
			// Bottom right corner.
			else if (x >= height - mask_halfsize && y >= width - mask_halfsize) {
				SetPixelValue(x, y, *(source_bitmap +
					(height - 2 * mask_halfsize - 1) * 3 * (width - 2 * mask_halfsize) + 3 * (width - 2 * mask_halfsize - 1)));
			}
			// Upper beam.
			else if (x < mask_halfsize) {
				SetPixelValue(x, y, *(source_bitmap + 3 * (y - mask_halfsize)));
			}
			// Bottom beam.
			else if (x >= height -  mask_halfsize) {
				SetPixelValue(x, y, *(source_bitmap +
					(height - 2 * mask_halfsize - 1) * 3 * (width - 2 * mask_halfsize) + 3 * (y - mask_halfsize)));
			}
			// Left beam.
			else if (y < mask_halfsize) {
				SetPixelValue(x, y, *(source_bitmap +
					(x - mask_halfsize) * 3 * (width - 2 * mask_halfsize)));
			}
			// Right beam.
			else if (y >= width - mask_halfsize) {
				SetPixelValue(x, y, *(source_bitmap +
					(x - mask_halfsize) * 3 * (width - 2 * mask_halfsize) + 3 * (width - 2 * mask_halfsize - 1)));
			}
			// The rest of the image.
			else {
				SetPixelValue(x, y, *(source_bitmap +
				              (x - mask_halfsize) * 3 * (width - 2 * mask_halfsize) + 3 * (y - mask_halfsize)));
			}
		}
	}
}

void CannyEdgeDetector::PostProcessImage()
{
	// Decreasing width and height.
	unsigned long i;
	height -= 2 * mask_halfsize;
	width -= 2 * mask_halfsize;

	// Shrinking image.
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			i = (unsigned long) (x * 3 * width + 3 * y);
			*(source_bitmap + i) =
			*(source_bitmap + i + 1) =
			*(source_bitmap + i + 2) = workspace_bitmap[(x + mask_halfsize) * (width + 2 * mask_halfsize) + (y + mask_halfsize)];
		}
	}
}

void CannyEdgeDetector::Luminance()
{
	unsigned long i;
	float gray_value, blue_value, green_value, red_value;

	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {

			// Current "B" pixel position in bitmap table (calculated with x and y values).
			i = (unsigned long) (x * 3 * width + 3 * y);

			// The order of bytes is BGR.
			blue_value  = *(source_bitmap + i);
			green_value = *(source_bitmap + i + 1);
			red_value   = *(source_bitmap + i + 2);

			// Standard equation from RGB to grayscale.
			gray_value = (uint8_t) (0.299 * red_value + 0.587 * green_value + 0.114 * blue_value);

			// Ultimately making picture grayscale.
			*(source_bitmap + i) =
				*(source_bitmap + i + 1) =
				*(source_bitmap + i + 2) = gray_value;
		}
	}
}

void CannyEdgeDetector::GaussianBlur(float sigma)
{
	// We already calculated mask size in PreProcessImage.
	long signed_mask_halfsize;
	signed_mask_halfsize = this->mask_halfsize;

	float *gaussianMask;
	gaussianMask = new float[mask_size * mask_size];

	for (int i = -signed_mask_halfsize; i <= signed_mask_halfsize; i++) {
		for (int j = -signed_mask_halfsize; j <= signed_mask_halfsize; j++) {
			gaussianMask[(i + signed_mask_halfsize) * mask_size + j + signed_mask_halfsize]
				= (1 / (2 * PI * sigma * sigma)) * exp(-(i * i + j * j ) / (2 * sigma * sigma));
		}
	}

	unsigned long i;
	unsigned long i_offset;
	int row_offset;
	int col_offset;
	float new_pixel;

	for (x = signed_mask_halfsize; x < height - signed_mask_halfsize; x++) {
		for (y = signed_mask_halfsize; y < width - signed_mask_halfsize; y++) {
			new_pixel = 0;
			for (row_offset = -signed_mask_halfsize; row_offset <= signed_mask_halfsize; row_offset++) {
				for (col_offset = -signed_mask_halfsize; col_offset <= signed_mask_halfsize; col_offset++) {
					i_offset = (unsigned long) ((x + row_offset) * width + (y + col_offset));
					new_pixel += (float) ((workspace_bitmap[i_offset])) * gaussianMask[(signed_mask_halfsize + row_offset) * mask_size + signed_mask_halfsize + col_offset];
				}
			}
			i = (unsigned long) (x * width + y);
			workspace_bitmap[i] = new_pixel;
		}
	}

	delete[] gaussianMask;
}

void CannyEdgeDetector::EdgeDetection()
{
	// Sobel masks.
	float Gx[9];
	Gx[0] = 1.0; Gx[1] = 0.0; Gx[2] = -1.0;
	Gx[3] = 2.0; Gx[4] = 0.0; Gx[5] = -2.0;
	Gx[6] = 1.0; Gx[7] = 0.0; Gx[8] = -1.0;
	float Gy[9];
	Gy[0] = -1.0; Gy[1] = -2.0; Gy[2] = -1.0;
	Gy[3] =  0.0; Gy[4] =  0.0; Gy[5] =  0.0;
	Gy[6] =  1.0; Gy[7] =  2.0; Gy[8] =  1.0;

	float value_gx, value_gy;

	float max = 0.0;
	float angle = 0.0;

	// Convolution.
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			value_gx = 0.0;
			value_gy = 0.0;

			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 3; l++) {
					value_gx += Gx[l * 3 + k] * GetPixelValue((x + 1) + (1 - k),
					                                          (y + 1) + (1 - l));
					value_gy += Gy[l * 3 + k] * GetPixelValue((x + 1) + (1 - k),
					                                          (y + 1) + (1 - l));
				}
			}

			edge_magnitude[x * width + y] = sqrt(value_gx * value_gx + value_gy * value_gy) / 4.0;

			// Maximum magnitude.
			max = edge_magnitude[x * width + y] > max ? edge_magnitude[x * width + y] : max;

			// Angle calculation.
			if ((value_gx != 0.0) || (value_gy != 0.0)) {
				angle = atan2(value_gy, value_gx) * 180.0 / PI;
			} else {
				angle = 0.0;
			}
			if (((angle > -22.5) && (angle <= 22.5)) ||
			    ((angle > 157.5) && (angle <= -157.5))) {
				edge_direction[x * width + y] = 0;
			} else if (((angle > 22.5) && (angle <= 67.5)) ||
			           ((angle > -157.5) && (angle <= -112.5))) {
				edge_direction[x * width + y] = 45;
			} else if (((angle > 67.5) && (angle <= 112.5)) ||
			           ((angle > -112.5) && (angle <= -67.5))) {
				edge_direction[x * width + y] = 90;
			} else if (((angle > 112.5) && (angle <= 157.5)) ||
			           ((angle > -67.5) && (angle <= -22.5))) {
				edge_direction[x * width + y] = 135;
			}
		}
	}

	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			edge_magnitude[x * width + y] =
			    255.0f * edge_magnitude[x * width + y] / max;
			SetPixelValue(x, y, edge_magnitude[x * width + y]);
		}
	}
}

void CannyEdgeDetector::NonMaxSuppression()
{
	float pixel_1 = 0;
	float pixel_2 = 0;
	float pixel;

	for (x = 1; x < height - 1; x++) {
		for (y = 1; y < width - 1; y++) {
			if (edge_direction[x * width + y] == 0) {
				pixel_1 = edge_magnitude[(x + 1) * width + y];
				pixel_2 = edge_magnitude[(x - 1) * width + y];
			} else if (edge_direction[x * width + y] == 45) {
				pixel_1 = edge_magnitude[(x + 1) * width + y - 1];
				pixel_2 = edge_magnitude[(x - 1) * width + y + 1];
			} else if (edge_direction[x * width + y] == 90) {
				pixel_1 = edge_magnitude[x * width + y - 1];
				pixel_2 = edge_magnitude[x * width + y + 1];
			} else if (edge_direction[x * width + y] == 135) {
				pixel_1 = edge_magnitude[(x + 1) * width + y + 1];
				pixel_2 = edge_magnitude[(x - 1) * width + y - 1];
			}
			pixel = edge_magnitude[x * width + y];
			if ((pixel >= pixel_1) && (pixel >= pixel_2)) {
				SetPixelValue(x, y, pixel);
			} else {
				SetPixelValue(x, y, 0);
			}
		}
	}

	bool change = true;
	while (change) {
		change = false;
		for (x = 1; x < height - 1; x++) {
			for (y = 1; y < width - 1; y++) {
				if (GetPixelValue(x, y) == 255) {
					if (GetPixelValue(x + 1, y) == 128) {
						change = true;
						SetPixelValue(x + 1, y, 255);
					}
					if (GetPixelValue(x - 1, y) == 128) {
						change = true;
						SetPixelValue(x - 1, y, 255);
					}
					if (GetPixelValue(x, y + 1) == 128) {
						change = true;
						SetPixelValue(x, y + 1, 255);
					}
					if (GetPixelValue(x, y - 1) == 128) {
						change = true;
						SetPixelValue(x, y - 1, 255);
					}
					if (GetPixelValue(x + 1, y + 1) == 128) {
						change = true;
						SetPixelValue(x + 1, y + 1, 255);
					}
					if (GetPixelValue(x - 1, y - 1) == 128) {
						change = true;
						SetPixelValue(x - 1, y - 1, 255);
					}
					if (GetPixelValue(x - 1, y + 1) == 128) {
						change = true;
						SetPixelValue(x - 1, y + 1, 255);
					}
					if (GetPixelValue(x + 1, y - 1) == 128) {
						change = true;
						SetPixelValue(x + 1, y - 1, 255);
					}
				}
			}
		}
		if (change) {
			for (x = height - 2; x > 0; x--) {
				for (y = width - 2; y > 0; y--) {
					if (GetPixelValue(x, y) == 255) {
						if (GetPixelValue(x + 1, y) == 128) {
							change = true;
							SetPixelValue(x + 1, y, 255);
						}
						if (GetPixelValue(x - 1, y) == 128) {
							change = true;
							SetPixelValue(x - 1, y, 255);
						}
						if (GetPixelValue(x, y + 1) == 128) {
							change = true;
							SetPixelValue(x, y + 1, 255);
						}
						if (GetPixelValue(x, y - 1) == 128) {
							change = true;
							SetPixelValue(x, y - 1, 255);
						}
						if (GetPixelValue(x + 1, y + 1) == 128) {
							change = true;
							SetPixelValue(x + 1, y + 1, 255);
						}
						if (GetPixelValue(x - 1, y - 1) == 128) {
							change = true;
							SetPixelValue(x - 1, y - 1, 255);
						}
						if (GetPixelValue(x - 1, y + 1) == 128) {
							change = true;
							SetPixelValue(x - 1, y + 1, 255);
						}
						if (GetPixelValue(x + 1, y - 1) == 128) {
							change = true;
							SetPixelValue(x + 1, y - 1, 255);
						}
					}
				}
			}
		}
	}

	// Suppression
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			if (GetPixelValue(x, y) == 128) {
				SetPixelValue(x, y, 0);
			}
		}
	}
}

void CannyEdgeDetector::Hysteresis(uint8_t lowThreshold, uint8_t highThreshold)
{
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			if (GetPixelValue(x, y) >= highThreshold) {
				SetPixelValue(x, y, 255);
				this->HysteresisRecursion(x, y, lowThreshold);
			}
		}
	}

	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			if (GetPixelValue(x, y) != 255) {
				SetPixelValue(x, y, 0);
			}
		}
	}
}

void CannyEdgeDetector::HysteresisRecursion(long x, long y, uint8_t lowThreshold)
{
	uint8_t value = 0;

	for (long x1 = x - 1; x1 <= x + 1; x1++) {
		for (long y1 = y - 1; y1 <= y + 1; y1++) {
			if ((x1 < height) & (y1 < width) & (x1 >= 0) & (y1 >= 0)
			    & (x1 != x) & (y1 != y)) {

				value = GetPixelValue(x1, y1);
				if (value != 255) {
					if (value >= lowThreshold) {
						SetPixelValue(x1, y1, 255);
						this->HysteresisRecursion(x1, y1, lowThreshold);
					}
					else {
						SetPixelValue(x1, y1, 0);
					}
				}
			}
		}
	}
}
