/**
 * \file      CannyEdgeDetector.h
 * \brief     Canny algorithm header file.
 * \details   This file is part of student project. Some parts of code may be
 *            influenced by various examples found on internet.
 * \author    resset <silentdemon@gmail.com>
 * \date      2006-2012
 * \copyright GNU General Public License, http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef _CANNYEDGEDETECTOR_H_
#define _CANNYEDGEDETECTOR_H_

typedef unsigned char uint8_t;

/**
 * \brief Canny algorithm class.
 *
 * Algorithm executes each step of Canny algorithm in one method,
 * ProcessImage. It operates on 24-bit RGB (BGR) bitmap.
 */
class CannyEdgeDetector
{
	public:
		/**
		 * \var Approximation of pi constant.
		 */
		static constexpr float PI = 3.14159265f;

		/**
		 * \brief Constructor, initializes some private variables.
		 */
		CannyEdgeDetector();

		/**
		 * \brief Destructor, unallocates memory.
		 */
		~CannyEdgeDetector();

		/**
		 * \brief Main method processing image.
		 *
		 * Executes Canny algorithm steps on source image and returns image of
		 * same size, same bit depth (24) that contains black background and
		 * edges marked with white.
		 *
		 * Steps:
		 * - conversion to grayscale,
		 * - Gaussian blurring with sigma parameter,
		 * - calculation of edge magnitude and direction,
		 * - suppression of non maximum pixels,
		 * - hysteresis thresholding between `lowThreshold` and `highThreshold`
		 *   values.
		 *
		 * Above steps are performed on image data organised in two-dimensional
		 * array of bytes which size is calculated as `width` * `height` * 3.
		 * Almost all of them are performed on `workspace_bitmap` which is
		 * higher and wider by few pixels than `source_bitmap`. This is because
		 * we need to have additional margins in order to make the steps that
		 * use masks work on every pixel of original image. For instance, Sobel
		 * mask is 3x3 so we need at least 1 pixel margin on every side. But the
		 * size of Gauss mask is variable, depending on sigma value. This is why
		 * the margins are calculated in `PreProcessImage()`. Original width and
		 * height values used in addressing pixels are also enlarged.
		 *
		 * In many places there are used x and y variables which are used as
		 * counters in addressing pixels in following manner:
		 *        y->
		 *        012345
		 *     x 0......
		 *     | 1......
		 *     v 2......
		 *       3......
		 *
		 * \param source_bitmap Source image.
		 * \param width Width of source image.
		 * \param height Height of source image.
		 * \param sigma Gaussian function standard deviation.
		 * \param lowThreshold Lower threshold of hysteresis (from range of 0-255).
		 * \param highThreshold Upper threshold of hysteresis (from range of 0-255).
		 * \return Destination image, bitmap containing edges found.
		 */
		uint8_t* ProcessImage(uint8_t* source_bitmap, unsigned int width,
		                      unsigned int height, float sigma = 1.0f,
		                      uint8_t lowThreshold = 30, uint8_t highThreshold = 80);

	private:
		/**
		 * \var Bitmap with source image.
		 */
		uint8_t *source_bitmap;

		/**
		 * \var Bitmap with image that algorithm is working on.
		 */
		uint8_t *workspace_bitmap;

		/**
		 * \var Array storing gradient magnitude.
		 */
		float *edge_magnitude;

		/**
		 * \var Array storing edge direction (0, 45, 90 and 135 degrees).
		 */
		uint8_t *edge_direction;

		/**
		 * \var Width of currently processed image, in pixels.
		 */
		unsigned int width;

		/**
		 * \var Height of currently processed image, in pixels.
		 */
		unsigned int height;

		/**
		 * \var Pixel x coordinate.
		 */
		unsigned int x;

		/**
		 * \var Pixel y coordinate.
		 */
		unsigned int y;

		/**
		 * \var Width of Gauss transform mask (kernel).
		 */
		unsigned int mask_size;

		/**
		 * \var Width of the margin (floor of half of the Gauss mask size).
		 */
		unsigned int mask_halfsize;

		/**
		 * \brief Gets value of (x, y) pixel.
		 *
		 * Operates only on `workspace_bitmap`.
		 *
		 * \param x Pixel x coordinate.
		 * \param y Pixel y coordinate.
		 * \return Pixel (x, y) value.
		 */
		inline uint8_t GetPixelValue(unsigned int x, unsigned int y);

		/**
		 * \brief Sets (x, y) pixel to certain value.
		 *
		 * Operates only on `workspace_bitmap`.
		 *
		 * \param x Pixel x coordinate.
		 * \param y Pixel y coordinate.
		 * \param value Pixel value (0-255).
		 */
		inline void SetPixelValue(unsigned int x, unsigned int y, uint8_t value);

		/**
		 * \brief Initializes arrays for use by the algorithm.
		 *
		 * \param sigma Parameter used for calculation of margin that the image
		 * must be enlarged with.
		 */
		void PreProcessImage(float sigma);

		/**
		 * \brief Cuts margins and returns image of original size.
		 */
		void PostProcessImage();

		/**
		 * \brief Converts image to grayscale.
		 *
		 * Information of chrominance are useless, we only need grayscale image.
		 */
		void Luminance();

		/**
		 * \brief Convolves image with Gauss filter - performs Gaussian blur.
		 *
		 * This step performs noise reduction algorithm.
		 *
		 * \param sigma Gaussian function standard deviation. The higher value,
		 * the stronger blur.
		 */
		void GaussianBlur(float sigma);

		/**
		 * \brief Calculates magnitude and direction of image gradient.
		 *
		 * Method saves results in two arrays, edge_magnitude and
		 * edge_direction.
		 */
		void EdgeDetection();

		/**
		 * \brief Deletes non-max pixels from gradient magnitude map.
		 *
		 * By using edge direction information this method looks for local
		 * maxima of gradient magnitude. As a result we get map with edges
		 * of 1 pixel width.
		 */
		void NonMaxSuppression();

		/**
		 * \brief Performs hysteresis thresholding between two values.
		 *
		 * \param lowThreshold Lower threshold of hysteresis (from range of 0-255).
		 * \param highThreshold Upper threshold of hysteresis (from range of 0-255).
		 */
		void Hysteresis(uint8_t lowThreshold, uint8_t highThreshold);

		/**
		 * \brief Support method in hysteresis thresholding operation.
		 *
		 * \param x Pixel x coordinate.
		 * \param y Pixel y coordinate.
		 * \param lowThreshold Lower threshold of hysteresis (from range of 0-255).
		 */
		void HysteresisRecursion(long x, long y, uint8_t lowThreshold);
};

#endif // #ifndef _CANNYEDGEDETECTOR_H_
