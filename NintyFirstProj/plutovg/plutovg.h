#ifndef PLUTOVG_H
#define PLUTOVG_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(PLUTOVG_BUILD_STATIC) && (defined(_WIN32) || defined(__CYGWIN__))
#define PLUTOVG_EXPORT __declspec(dllexport)
#define PLUTOVG_IMPORT __declspec(dllimport)
#elif defined(__GNUC__) && (__GNUC__ >= 4)
#define PLUTOVG_EXPORT __attribute__((__visibility__("default")))
#define PLUTOVG_IMPORT
#else
#define PLUTOVG_EXPORT
#define PLUTOVG_IMPORT
#endif

#ifdef PLUTOVG_BUILD
#define PLUTOVG_API PLUTOVG_EXPORT
#else
#define PLUTOVG_API PLUTOVG_IMPORT
#endif

#define PLUTOVG_VERSION_MAJOR 0
#define PLUTOVG_VERSION_MINOR 0
#define PLUTOVG_VERSION_MICRO 1

#define PLUTOVG_VERSION_ENCODE(major, minor, micro) (((major) * 10000) + ((minor) * 100) + ((micro) * 1))
#define PLUTOVG_VERSION PLUTOVG_VERSION_ENCODE(PLUTOVG_VERSION_MAJOR, PLUTOVG_VERSION_MINOR, PLUTOVG_VERSION_MICRO)

#define PLUTOVG_VERSION_XSTRINGIZE(major, minor, micro) #major"."#minor"."#micro
#define PLUTOVG_VERSION_STRINGIZE(major, minor, micro) PLUTOVG_VERSION_XSTRINGIZE(major, minor, micro)
#define PLUTOVG_VERSION_STRING PLUTOVG_VERSION_STRINGIZE(PLUTOVG_VERSION_MAJOR, PLUTOVG_VERSION_MINOR, PLUTOVG_VERSION_MICRO)

/**
 * @brief Gets the version of the plutovg library.
 * @return An integer representing the version of the plutovg library.
 */
PLUTOVG_API int plutovg_version(void);

/**
 * @brief Gets the version of the plutovg library as a string.
 * @return A string representing the version of the plutovg library.
 */
PLUTOVG_API const char* plutovg_version_string(void);

/**
 * @brief A function pointer type for a cleanup callback.
 * @param closure A pointer to the resource to be cleaned up.
 */
typedef void (*plutovg_destroy_func_t)(void* closure);

/**
 * @brief A function pointer type for a write callback.
 * @param closure A pointer to user-defined data or context.
 * @param data A pointer to the data to be written.
 * @param size The size of the data in bytes.
 */
typedef void (*plutovg_write_func_t)(void* closure, void* data, int size);

/**
 * @brief A structure representing a point in 2D space.
 */
typedef struct plutovg_point {
    float x; /**< The x-coordinate of the point. */
    float y; /**< The y-coordinate of the point. */
} plutovg_point_t;

/**
 * @brief A structure representing a rectangle in 2D space.
 */
typedef struct plutovg_rect {
    float x; /**< The x-coordinate of the top-left corner of the rectangle. */
    float y; /**< The y-coordinate of the top-left corner of the rectangle. */
    float w; /**< The width of the rectangle. */
    float h; /**< The height of the rectangle. */
} plutovg_rect_t;

/**
 * @brief A structure representing a 2D transformation matrix.
 */
typedef struct plutovg_matrix {
    float a; /**< The horizontal scaling factor. */
    float b; /**< The vertical shearing factor. */
    float c; /**< The horizontal shearing factor. */
    float d; /**< The vertical scaling factor. */
    float e; /**< The horizontal translation offset. */
    float f; /**< The vertical translation offset. */
} plutovg_matrix_t;

/**
 * @brief Initializes a 2D transformation matrix.
 * @param matrix A pointer to the `plutovg_matrix_t` structure to be initialized.
 * @param a The horizontal scaling factor.
 * @param b The vertical shearing factor.
 * @param c The horizontal shearing factor.
 * @param d The vertical scaling factor.
 * @param e The horizontal translation offset.
 * @param f The vertical translation offset.
 */
PLUTOVG_API void plutovg_matrix_init(plutovg_matrix_t* matrix, float a, float b, float c, float d, float e, float f);

/**
 * @brief Initializes a 2D transformation matrix to the identity matrix.
 * @param matrix A pointer to the `plutovg_matrix_t` structure to be initialized.
 */
PLUTOVG_API void plutovg_matrix_init_identity(plutovg_matrix_t* matrix);

/**
 * @brief Initializes a 2D transformation matrix for translation.
 * @param matrix A pointer to the `plutovg_matrix_t` structure to be initialized.
 * @param tx The translation offset in the x-direction.
 * @param ty The translation offset in the y-direction.
 */
PLUTOVG_API void plutovg_matrix_init_translate(plutovg_matrix_t* matrix, float tx, float ty);

/**
 * @brief Initializes a 2D transformation matrix for scaling.
 * @param matrix A pointer to the `plutovg_matrix_t` structure to be initialized.
 * @param sx The scaling factor in the x-direction.
 * @param sy The scaling factor in the y-direction.
 */
PLUTOVG_API void plutovg_matrix_init_scale(plutovg_matrix_t* matrix, float sx, float sy);

/**
 * @brief Initializes a 2D transformation matrix for shearing.
 * @param matrix A pointer to the `plutovg_matrix_t` structure to be initialized.
 * @param shx The shearing factor in the x-direction.
 * @param shy The shearing factor in the y-direction.
 */
PLUTOVG_API void plutovg_matrix_init_shear(plutovg_matrix_t* matrix, float shx, float shy);

/**
 * @brief Initializes a 2D transformation matrix for rotation.
 * @param matrix A pointer to the `plutovg_matrix_t` structure to be initialized.
 * @param angle The rotation angle in radians.
 */
PLUTOVG_API void plutovg_matrix_init_rotate(plutovg_matrix_t* matrix, float angle);

/**
 * @brief Adds a translation with offsets `tx` and `ty` to the matrix.
 * @param matrix A pointer to the `plutovg_matrix_t` structure to be modified.
 * @param tx The translation offset in the x-direction.
 * @param ty The translation offset in the y-direction.
 */
PLUTOVG_API void plutovg_matrix_translate(plutovg_matrix_t* matrix, float tx, float ty);

/**
 * @brief Scales the matrix by factors `sx` and `sy`
 * @param matrix A pointer to the `plutovg_matrix_t` structure to be modified.
 * @param sx The scaling factor in the x-direction.
 * @param sy The scaling factor in the y-direction.
 */
PLUTOVG_API void plutovg_matrix_scale(plutovg_matrix_t* matrix, float sx, float sy);

/**
 * @brief Shears the matrix by factors `shx` and `shy`.
 * @param matrix A pointer to the `plutovg_matrix_t` structure to be modified.
 * @param shx The shearing factor in the x-direction.
 * @param shy The shearing factor in the y-direction.
 */
PLUTOVG_API void plutovg_matrix_shear(plutovg_matrix_t* matrix, float shx, float shy);

/**
 * @brief Rotates the matrix by the specified angle (in radians).
 * @param matrix A pointer to the `plutovg_matrix_t` structure to be modified.
 * @param angle The rotation angle in radians.
 */
PLUTOVG_API void plutovg_matrix_rotate(plutovg_matrix_t* matrix, float angle);

/**
 * @brief Multiplies `left` and `right` matrices and stores the result in `matrix`.
 * @param matrix A pointer to the `plutovg_matrix_t` structure to store the result.
 * @param left A pointer to the first `plutovg_matrix_t` matrix.
 * @param right A pointer to the second `plutovg_matrix_t` matrix.
 */
PLUTOVG_API void plutovg_matrix_multiply(plutovg_matrix_t* matrix, const plutovg_matrix_t* left, const plutovg_matrix_t* right);

/**
 * @brief Calculates the inverse of `matrix` and stores it in `inverse`.
 * @note If `inverse` is `NULL`, the function only checks if the matrix is invertible.
 * @param matrix A pointer to the `plutovg_matrix_t` matrix to invert.
 * @param inverse A pointer to the `plutovg_matrix_t` structure to store the result, or `NULL`.
 * @return `true` if the matrix is invertible; `false` otherwise.
 */
PLUTOVG_API bool plutovg_matrix_invert(const plutovg_matrix_t* matrix, plutovg_matrix_t* inverse);

/**
 * @brief Transforms the point `(x, y)` using `matrix` and stores the result in `(xx, yy)`.
 * @param matrix A pointer to the `plutovg_matrix_t` transformation matrix.
 * @param x The x-coordinate of the point to transform.
 * @param y The y-coordinate of the point to transform.
 * @param xx A pointer to store the transformed x-coordinate.
 * @param yy A pointer to store the transformed y-coordinate.
 */
PLUTOVG_API void plutovg_matrix_map(const plutovg_matrix_t* matrix, float x, float y, float* xx, float* yy);

/**
 * @brief Transforms the `src` point using `matrix` and stores the result in `dst`.
 * @param matrix A pointer to the `plutovg_matrix_t` transformation matrix.
 * @param src A pointer to the `plutovg_point_t` point to transform.
 * @param dst A pointer to the `plutovg_point_t` to store the transformed point.
 */
PLUTOVG_API void plutovg_matrix_map_point(const plutovg_matrix_t* matrix, const plutovg_point_t* src, plutovg_point_t* dst);

/**
 * @brief Transforms an array of `src` points using `matrix` and stores the results in `dst`.
 * @param matrix A pointer to the `plutovg_matrix_t` transformation matrix.
 * @param src A pointer to the array of `plutovg_point_t` points to transform.
 * @param dst A pointer to the array of `plutovg_point_t` to store the transformed points.
 * @param count The number of points to transform.
 */
PLUTOVG_API void plutovg_matrix_map_points(const plutovg_matrix_t* matrix, const plutovg_point_t* src, plutovg_point_t* dst, int count);

/**
 * @brief Transforms the `src` rectangle using `matrix` and stores the result in `dst`.
 * @param matrix A pointer to the `plutovg_matrix_t` transformation matrix.
 * @param src A pointer to the `plutovg_rect_t` rectangle to transform.
 * @param dst A pointer to the `plutovg_rect_t` to store the transformed rectangle.
 */
PLUTOVG_API void plutovg_matrix_map_rect(const plutovg_matrix_t* matrix, const plutovg_rect_t* src, plutovg_rect_t* dst);

typedef struct plutovg_path plutovg_path_t;

/**
 * @brief Enumeration defining path commands
 */
typedef enum plutovg_path_command {
    PLUTOVG_PATH_COMMAND_MOVE_TO, /**< Moves the current point to a new position. */
    PLUTOVG_PATH_COMMAND_LINE_TO, /**< Draws a straight line to a new point. */
    PLUTOVG_PATH_COMMAND_CUBIC_TO, /**< Draws a cubic Bézier curve to a new point. */
    PLUTOVG_PATH_COMMAND_CLOSE /**< Closes the current path by drawing a line to the starting point. */
} plutovg_path_command_t;

/**
 * @brief Union representing a path element.
 *
 * A path element can be a command with a length or a coordinate point.
 * Each command type in the path element array is followed by a specific number of points:
 * - `PLUTOVG_PATH_COMMAND_MOVE_TO`: 1 point
 * - `PLUTOVG_PATH_COMMAND_LINE_TO`: 1 point
 * - `PLUTOVG_PATH_COMMAND_CUBIC_TO`: 3 points
 * - `PLUTOVG_PATH_COMMAND_CLOSE`: 1 point
 *
 * @example
 * const plutovg_path_element_t* elements;
 * int count = plutovg_path_get_elements(path, &elements);
 * for (int i = 0; i < count; i += elements[i].header.length) {
 *     plutovg_path_command_t command = elements[i].header.command;
 *     switch (command) {
 *         case PLUTOVG_PATH_COMMAND_MOVE_TO:
 *             printf("MoveTo: %g %g\n", elements[i + 1].point.x, elements[i + 1].point.y);
 *             break;
 *         case PLUTOVG_PATH_COMMAND_LINE_TO:
 *             printf("LineTo: %g %g\n", elements[i + 1].point.x, elements[i + 1].point.y);
 *             break;
 *         case PLUTOVG_PATH_COMMAND_CUBIC_TO:
 *             printf("CubicTo: %g %g %g %g %g %g\n",
 *                    elements[i + 1].point.x, elements[i + 1].point.y,
 *                    elements[i + 2].point.x, elements[i + 2].point.y,
 *                    elements[i + 3].point.x, elements[i + 3].point.y);
 *             break;
 *         case PLUTOVG_PATH_COMMAND_CLOSE:
 *             printf("Close: %g %g\n", elements[i + 1].point.x, elements[i + 1].point.y);
 *             break;
 *     }
 * }
 */
typedef union plutovg_path_element {
    struct {
        plutovg_path_command_t command; /**< The path command. */
        int length; /**< Number of elements including the header. */
    } header; /**< Header for path commands. */
    plutovg_point_t point; /**< A coordinate point in the path. */
} plutovg_path_element_t;

/**
 * @brief Iterator for traversing path elements in a path.
 */
typedef struct plutovg_path_iterator {
    const plutovg_path_element_t* elements; /**< Pointer to the array of path elements. */
    int size; /**< Total number of elements in the array. */
    int index; /**< Current position in the array. */
} plutovg_path_iterator_t;

/**
 * @brief Initializes the path iterator for a given path.
 *
 * @param it The path iterator to initialize.
 * @param path The path to iterate over.
 */
PLUTOVG_API void plutovg_path_iterator_init(plutovg_path_iterator_t* it, const plutovg_path_t* path);

/**
 * @brief Checks if there are more elements to iterate over.
 *
 * @param it The path iterator.
 * @return `true` if there are more elements; otherwise, `false`.
 */
PLUTOVG_API bool plutovg_path_iterator_has_next(const plutovg_path_iterator_t* it);

/**
 * @brief Retrieves the current command and its associated points, and advances the iterator.
 *
 * @param it The path iterator.
 * @param points Array to store the points for the current command.
 * @return The path command for the current element.
 */
PLUTOVG_API plutovg_path_command_t plutovg_path_iterator_next(plutovg_path_iterator_t* it, plutovg_point_t points[3]);

/**
 * @brief Creates a new path object.
 *
 * Initializes and returns a pointer to a new `plutovg_path_t` object.
 * The path object should be released using `plutovg_path_destroy()` when no longer needed.
 *
 * @return A pointer to the newly created path object.
 */
PLUTOVG_API plutovg_path_t* plutovg_path_create(void);

/**
 * @brief Increases the reference count of a path object.
 *
 * This function increments the reference count of the given path object,
 * ensuring that it remains valid as long as it is in use. The reference count
 * should eventually be decremented using `plutovg_path_destroy()`.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @return A pointer to the same `plutovg_path_t` object.
 */
PLUTOVG_API plutovg_path_t* plutovg_path_reference(plutovg_path_t* path);

/**
 * @brief Decreases the reference count of a path object.
 *
 * This function decrements the reference count of the given path object. If
 * the reference count reaches zero, the path object is destroyed and its
 * resources are freed.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 */
PLUTOVG_API void plutovg_path_destroy(plutovg_path_t* path);

/**
 * @brief Retrieves the reference count of a path object.
 *
 * This function returns the current reference count of the given path object.
 * The reference count reflects how many references exist to the path object.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @return The current reference count of the path object.
 */
PLUTOVG_API int plutovg_path_get_reference_count(const plutovg_path_t* path);

/**
 * @brief Retrieves the elements of a path.
 *
 * This function provides access to the array of path elements.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param elements A pointer to a pointer that will be set to point to the array of path elements.
 * @return The number of elements in the path.
 */
PLUTOVG_API int plutovg_path_get_elements(const plutovg_path_t* path, const plutovg_path_element_t** elements);

/**
 * @brief Moves the current point to a new position.
 *
 * This function moves the current point to the specified coordinates without
 * drawing a line. This is equivalent to the `M` command in SVG path syntax.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param x The x-coordinate of the new position.
 * @param y The y-coordinate of the new position.
 */
PLUTOVG_API void plutovg_path_move_to(plutovg_path_t* path, float x, float y);

/**
 * @brief Adds a straight line segment to the path.
 *
 * This function adds a straight line segment from the current point to the
 * specified coordinates. This is equivalent to the `L` command in SVG path syntax.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param x The x-coordinate of the end point of the line segment.
 * @param y The y-coordinate of the end point of the line segment.
 */
PLUTOVG_API void plutovg_path_line_to(plutovg_path_t* path, float x, float y);

/**
 * @brief Adds a quadratic Bézier curve to the path.
 *
 * This function adds a quadratic Bézier curve segment from the current point
 * to the specified end point, using the given control point. This is equivalent
 * to the `Q` command in SVG path syntax.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param x1 The x-coordinate of the control point.
 * @param y1 The y-coordinate of the control point.
 * @param x2 The x-coordinate of the end point of the curve.
 * @param y2 The y-coordinate of the end point of the curve.
 */
PLUTOVG_API void plutovg_path_quad_to(plutovg_path_t* path, float x1, float y1, float x2, float y2);

/**
 * @brief Adds a cubic Bézier curve to the path.
 *
 * This function adds a cubic Bézier curve segment from the current point
 * to the specified end point, using the given two control points. This is
 * equivalent to the `C` command in SVG path syntax.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param x1 The x-coordinate of the first control point.
 * @param y1 The y-coordinate of the first control point.
 * @param x2 The x-coordinate of the second control point.
 * @param y2 The y-coordinate of the second control point.
 * @param x3 The x-coordinate of the end point of the curve.
 * @param y3 The y-coordinate of the end point of the curve.
 */
PLUTOVG_API void plutovg_path_cubic_to(plutovg_path_t* path, float x1, float y1, float x2, float y2, float x3, float y3);

/**
 * @brief Adds an elliptical arc to the path.
 *
 * This function adds an elliptical arc segment from the current point to the
 * specified end point. The arc is defined by the radii, rotation angle, and
 * flags for large arc and sweep. This is equivalent to the `A` command in SVG
 * path syntax.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param rx The x-radius of the ellipse.
 * @param ry The y-radius of the ellipse.
 * @param angle The rotation angle of the ellipse in radians.
 * @param large_arc_flag If true, draw the large arc; otherwise, draw the small arc.
 * @param sweep_flag If true, draw the arc in the positive-angle direction; otherwise, in the negative-angle direction.
 * @param x The x-coordinate of the end point of the arc.
 * @param y The y-coordinate of the end point of the arc.
 */
PLUTOVG_API void plutovg_path_arc_to(plutovg_path_t* path, float rx, float ry, float angle, bool large_arc_flag, bool sweep_flag, float x, float y);

/**
 * @brief Closes the current sub-path.
 *
 * This function closes the current sub-path by drawing a straight line back to
 * the start point of the sub-path. This is equivalent to the `Z` command in SVG
 * path syntax.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 */
PLUTOVG_API void plutovg_path_close(plutovg_path_t* path);

/**
 * @brief Retrieves the current point of the path.
 *
 * Gets the current point's coordinates in the path. This point is the last
 * position used or the point where the path was last moved to.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param x Output parameter for the x-coordinate of the current point.
 * @param y Output parameter for the y-coordinate of the current point.
 */
PLUTOVG_API void plutovg_path_get_current_point(plutovg_path_t* path, float* x, float* y);

/**
 * @brief Reserves space for path elements.
 *
 * Reserves space for a specified number of elements in the path. This helps optimize
 * memory allocation for future path operations.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param count The number of path elements to reserve space for.
 */
PLUTOVG_API void plutovg_path_reserve(plutovg_path_t* path, int count);

/**
 * @brief Resets the path.
 *
 * Clears all path data, effectively resetting the `plutovg_path_t` object to its initial state.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 */
PLUTOVG_API void plutovg_path_reset(plutovg_path_t* path);

/**
 * @brief Adds a rectangle to the path.
 *
 * Adds a rectangle defined by the top-left corner (x, y) and dimensions (w, h) to the path.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param x The x-coordinate of the rectangle's top-left corner.
 * @param y The y-coordinate of the rectangle's top-left corner.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 */
PLUTOVG_API void plutovg_path_add_rect(plutovg_path_t* path, float x, float y, float w, float h);

/**
 * @brief Adds a rounded rectangle to the path.
 *
 * Adds a rounded rectangle defined by the top-left corner (x, y), dimensions (w, h),
 * and corner radii (rx, ry) to the path.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param x The x-coordinate of the rectangle's top-left corner.
 * @param y The y-coordinate of the rectangle's top-left corner.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param rx The x-radius of the rectangle's corners.
 * @param ry The y-radius of the rectangle's corners.
 */
PLUTOVG_API void plutovg_path_add_round_rect(plutovg_path_t* path, float x, float y, float w, float h, float rx, float ry);

/**
 * @brief Adds an ellipse to the path.
 *
 * Adds an ellipse defined by the center (cx, cy) and radii (rx, ry) to the path.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param cx The x-coordinate of the ellipse's center.
 * @param cy The y-coordinate of the ellipse's center.
 * @param rx The x-radius of the ellipse.
 * @param ry The y-radius of the ellipse.
 */
PLUTOVG_API void plutovg_path_add_ellipse(plutovg_path_t* path, float cx, float cy, float rx, float ry);

/**
 * @brief Adds a circle to the path.
 *
 * Adds a circle defined by its center (cx, cy) and radius (r) to the path.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param cx The x-coordinate of the circle's center.
 * @param cy The y-coordinate of the circle's center.
 * @param r The radius of the circle.
 */
PLUTOVG_API void plutovg_path_add_circle(plutovg_path_t* path, float cx, float cy, float r);

/**
 * @brief Adds an arc to the path.
 *
 * Adds an arc defined by the center (cx, cy), radius (r), start angle (a0), end angle (a1),
 * and direction (ccw) to the path.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param cx The x-coordinate of the arc's center.
 * @param cy The y-coordinate of the arc's center.
 * @param r The radius of the arc.
 * @param a0 The start angle of the arc in radians.
 * @param a1 The end angle of the arc in radians.
 * @param ccw If true, the arc is drawn counter-clockwise; if false, clockwise.
 */
PLUTOVG_API void plutovg_path_add_arc(plutovg_path_t* path, float cx, float cy, float r, float a0, float a1, bool ccw);

/**
 * @brief Adds a sub-path to the path.
 *
 * Adds all elements from another path (`source`) to the current path, optionally
 * applying a transformation matrix.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param source A pointer to the `plutovg_path_t` object to copy elements from.
 * @param matrix A pointer to a `plutovg_matrix_t` transformation matrix, or `NULL` to apply no transformation.
 */
PLUTOVG_API void plutovg_path_add_path(plutovg_path_t* path, const plutovg_path_t* source, const plutovg_matrix_t* matrix);

/**
 * @brief Applies a transformation matrix to the path.
 *
 * Transforms the entire path using the provided transformation matrix.
 *
 * @param path A pointer to the `plutovg_path_t` object.
 * @param matrix A pointer to the `plutovg_matrix_t` transformation matrix.
 */
PLUTOVG_API void plutovg_path_transform(plutovg_path_t* path, const plutovg_matrix_t* matrix);

typedef void (*plutovg_path_traverse_func_t)(void* closure, plutovg_path_command_t command, const plutovg_point_t* points, int npoints);

PLUTOVG_API void plutovg_path_traverse(const plutovg_path_t* path, plutovg_path_traverse_func_t traverse_func, void* closure);
PLUTOVG_API void plutovg_path_traverse_flatten(const plutovg_path_t* path, plutovg_path_traverse_func_t traverse_func, void* closure);
PLUTOVG_API void plutovg_path_traverse_dashed(const plutovg_path_t* path, float offset, const float* dashes, int ndashes, plutovg_path_traverse_func_t traverse_func, void* closure);

PLUTOVG_API float plutovg_path_extents(const plutovg_path_t* path, plutovg_rect_t* extents);
PLUTOVG_API float plutovg_path_length(const plutovg_path_t* path);

PLUTOVG_API plutovg_path_t* plutovg_path_clone(const plutovg_path_t* path);
PLUTOVG_API plutovg_path_t* plutovg_path_clone_flatten(const plutovg_path_t* path);
PLUTOVG_API plutovg_path_t* plutovg_path_clone_dashed(const plutovg_path_t* path, float offset, const float* dashes, int ndashes);

PLUTOVG_API bool plutovg_path_parse(plutovg_path_t* path, const char* data, int length);

typedef enum plutovg_text_encoding {
    PLUTOVG_TEXT_ENCODING_UTF8,
    PLUTOVG_TEXT_ENCODING_UTF16,
    PLUTOVG_TEXT_ENCODING_UTF32,
    PLUTOVG_TEXT_ENCODING_LATIN1
} plutovg_text_encoding_t;

typedef struct plutovg_text_iterator {
    const void* text;
    int length;
    plutovg_text_encoding_t encoding;
    int index;
} plutovg_text_iterator_t;

typedef unsigned int plutovg_codepoint_t;

PLUTOVG_API void plutovg_text_iterator_init(plutovg_text_iterator_t* it, const void* text, int length, plutovg_text_encoding_t encoding);
PLUTOVG_API bool plutovg_text_iterator_has_next(const plutovg_text_iterator_t* it);
PLUTOVG_API plutovg_codepoint_t plutovg_text_iterator_next(plutovg_text_iterator_t* it);

typedef struct plutovg_font_face plutovg_font_face_t;

PLUTOVG_API plutovg_font_face_t* plutovg_font_face_load_from_file(const char* filename, int ttcindex);
PLUTOVG_API plutovg_font_face_t* plutovg_font_face_load_from_data(const void* data, unsigned int length, int ttcindex, plutovg_destroy_func_t destroy_func, void* closure);

PLUTOVG_API plutovg_font_face_t* plutovg_font_face_reference(plutovg_font_face_t* face);
PLUTOVG_API void plutovg_font_face_destroy(plutovg_font_face_t* face);
PLUTOVG_API int plutovg_font_face_get_reference_count(const plutovg_font_face_t* face);

PLUTOVG_API void plutovg_font_face_get_metrics(const plutovg_font_face_t* face, float size, float* ascent, float* descent, float* line_gap, plutovg_rect_t* extents);
PLUTOVG_API void plutovg_font_face_get_glyph_metrics(const plutovg_font_face_t* face, float size, plutovg_codepoint_t codepoint, float* advance_width, float* left_side_bearing, plutovg_rect_t* extents);

PLUTOVG_API float plutovg_font_face_get_glyph_path(const plutovg_font_face_t* face, float size, float x, float y, plutovg_codepoint_t codepoint, plutovg_path_t* path);
PLUTOVG_API float plutovg_font_face_traverse_glyph_path(const plutovg_font_face_t* face, float size, float x, float y, plutovg_codepoint_t codepoint, plutovg_path_traverse_func_t traverse_func, void* closure);

PLUTOVG_API float plutovg_font_face_text_extents(const plutovg_font_face_t* face, float size, const void* text, int length, plutovg_text_encoding_t encoding, plutovg_rect_t* extents);

/**
 * @note plutovg_surface_t format is ARGB32_Premultiplied.
 */
typedef struct plutovg_surface plutovg_surface_t;

PLUTOVG_API plutovg_surface_t* plutovg_surface_create(int width, int height);
PLUTOVG_API plutovg_surface_t* plutovg_surface_create_for_data(unsigned char* data, int width, int height, int stride);

PLUTOVG_API plutovg_surface_t* plutovg_surface_load_from_image_file(const char* filename);
PLUTOVG_API plutovg_surface_t* plutovg_surface_load_from_image_data(const void* data, int length);
PLUTOVG_API plutovg_surface_t* plutovg_surface_load_from_image_base64(const char* data, int length);

PLUTOVG_API plutovg_surface_t* plutovg_surface_reference(plutovg_surface_t* surface);
PLUTOVG_API void plutovg_surface_destroy(plutovg_surface_t* surface);
PLUTOVG_API int plutovg_surface_get_reference_count(const plutovg_surface_t* surface);

PLUTOVG_API unsigned char* plutovg_surface_get_data(const plutovg_surface_t* surface);
PLUTOVG_API int plutovg_surface_get_width(const plutovg_surface_t* surface);
PLUTOVG_API int plutovg_surface_get_height(const plutovg_surface_t* surface);
PLUTOVG_API int plutovg_surface_get_stride(const plutovg_surface_t* surface);

PLUTOVG_API bool plutovg_surface_write_to_png(const plutovg_surface_t* surface, const char* filename);
PLUTOVG_API bool plutovg_surface_write_to_jpg(const plutovg_surface_t* surface, const char* filename, int quality);

PLUTOVG_API bool plutovg_surface_write_to_png_stream(const plutovg_surface_t* surface, plutovg_write_func_t write_func, void* closure);
PLUTOVG_API bool plutovg_surface_write_to_jpg_stream(const plutovg_surface_t* surface, plutovg_write_func_t write_func, void* closure, int quality);

PLUTOVG_API void plutovg_convert_argb_to_rgba(unsigned char* dst, const unsigned char* src, int width, int height, int stride);
PLUTOVG_API void plutovg_convert_rgba_to_argb(unsigned char* dst, const unsigned char* src, int width, int height, int stride);

typedef struct plutovg_color {
    float r;
    float g;
    float b;
    float a;
} plutovg_color_t;

typedef enum {
    PLUTOVG_TEXTURE_TYPE_PLAIN,
    PLUTOVG_TEXTURE_TYPE_TILED
} plutovg_texture_type_t;

typedef enum {
    PLUTOVG_SPREAD_METHOD_PAD,
    PLUTOVG_SPREAD_METHOD_REFLECT,
    PLUTOVG_SPREAD_METHOD_REPEAT
} plutovg_spread_method_t;

typedef struct plutovg_gradient_stop {
    float offset;
    plutovg_color_t color;
} plutovg_gradient_stop_t;

typedef struct plutovg_paint plutovg_paint_t;

PLUTOVG_API plutovg_paint_t* plutovg_paint_create_rgb(float r, float g, float b);
PLUTOVG_API plutovg_paint_t* plutovg_paint_create_rgba(float r, float g, float b, float a);
PLUTOVG_API plutovg_paint_t* plutovg_paint_create_color(const plutovg_color_t* color);
PLUTOVG_API plutovg_paint_t* plutovg_paint_create_linear_gradient(float x1, float y1, float x2, float y2,
    plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix);
PLUTOVG_API plutovg_paint_t* plutovg_paint_create_radial_gradient(float cx, float cy, float cr, float fx, float fy, float fr,
    plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix);
PLUTOVG_API plutovg_paint_t* plutovg_paint_create_texture(plutovg_surface_t* surface, plutovg_texture_type_t type, float opacity, const plutovg_matrix_t* matrix);

PLUTOVG_API plutovg_paint_t* plutovg_paint_reference(plutovg_paint_t* paint);
PLUTOVG_API void plutovg_paint_destroy(plutovg_paint_t* paint);
PLUTOVG_API int plutovg_paint_get_reference_count(const plutovg_paint_t* paint);

typedef enum {
    PLUTOVG_FILL_RULE_NON_ZERO,
    PLUTOVG_FILL_RULE_EVEN_ODD
} plutovg_fill_rule_t;

typedef enum {
    PLUTOVG_OPERATOR_SRC,
    PLUTOVG_OPERATOR_SRC_OVER,
    PLUTOVG_OPERATOR_DST_IN,
    PLUTOVG_OPERATOR_DST_OUT
} plutovg_operator_t;

typedef enum {
    PLUTOVG_LINE_CAP_BUTT,
    PLUTOVG_LINE_CAP_ROUND,
    PLUTOVG_LINE_CAP_SQUARE
} plutovg_line_cap_t;

typedef enum {
    PLUTOVG_LINE_JOIN_MITER,
    PLUTOVG_LINE_JOIN_ROUND,
    PLUTOVG_LINE_JOIN_BEVEL
} plutovg_line_join_t;

typedef struct plutovg_canvas plutovg_canvas_t;

PLUTOVG_API plutovg_canvas_t* plutovg_canvas_create(plutovg_surface_t* surface);

PLUTOVG_API plutovg_canvas_t* plutovg_canvas_reference(plutovg_canvas_t* canvas);
PLUTOVG_API void plutovg_canvas_destroy(plutovg_canvas_t* canvas);
PLUTOVG_API int plutovg_canvas_get_reference_count(const plutovg_canvas_t* canvas);

PLUTOVG_API void plutovg_canvas_save(plutovg_canvas_t* canvas);
PLUTOVG_API void plutovg_canvas_restore(plutovg_canvas_t* canvas);

PLUTOVG_API void plutovg_canvas_set_rgb(plutovg_canvas_t* canvas, float r, float g, float b);
PLUTOVG_API void plutovg_canvas_set_rgba(plutovg_canvas_t* canvas, float r, float g, float b, float a);
PLUTOVG_API void plutovg_canvas_set_color(plutovg_canvas_t* canvas, const plutovg_color_t* color);
PLUTOVG_API void plutovg_canvas_set_linear_gradient(plutovg_canvas_t* canvas, float x1, float y1, float x2, float y2,
    plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix);
PLUTOVG_API void plutovg_canvas_set_radial_gradient(plutovg_canvas_t* canvas, float cx, float cy, float cr, float fx, float fy, float fr,
    plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix);
PLUTOVG_API void plutovg_canvas_set_texture(plutovg_canvas_t* canvas, plutovg_surface_t* surface, plutovg_texture_type_t type, float opacity, const plutovg_matrix_t* matrix);

PLUTOVG_API void plutovg_canvas_set_paint(plutovg_canvas_t* canvas, plutovg_paint_t* paint);
PLUTOVG_API plutovg_paint_t* plutovg_canvas_get_paint(const plutovg_canvas_t* canvas);

PLUTOVG_API void plutovg_canvas_set_font(plutovg_canvas_t* canvas, plutovg_font_face_t* face, float size);
PLUTOVG_API void plutovg_canvas_set_font_face(plutovg_canvas_t* canvas, plutovg_font_face_t* face);
PLUTOVG_API void plutovg_canvas_set_font_size(plutovg_canvas_t* canvas, float size);

PLUTOVG_API plutovg_font_face_t* plutovg_canvas_get_font_face(const plutovg_canvas_t* canvas);
PLUTOVG_API float plutovg_canvas_get_font_size(const plutovg_canvas_t* canvas);

PLUTOVG_API void plutovg_canvas_set_fill_rule(plutovg_canvas_t* canvas, plutovg_fill_rule_t winding);
PLUTOVG_API void plutovg_canvas_set_operator(plutovg_canvas_t* canvas, plutovg_operator_t op);
PLUTOVG_API void plutovg_canvas_set_opacity(plutovg_canvas_t* canvas, float opacity);

PLUTOVG_API plutovg_fill_rule_t plutovg_canvas_get_fill_rule(const plutovg_canvas_t* canvas);
PLUTOVG_API plutovg_operator_t plutovg_canvas_get_operator(const plutovg_canvas_t* canvas);
PLUTOVG_API float plutovg_canvas_get_opacity(const plutovg_canvas_t* canvas);

PLUTOVG_API void plutovg_canvas_set_line_width(plutovg_canvas_t* canvas, float line_width);
PLUTOVG_API void plutovg_canvas_set_line_cap(plutovg_canvas_t* canvas, plutovg_line_cap_t line_cap);
PLUTOVG_API void plutovg_canvas_set_line_join(plutovg_canvas_t* canvas, plutovg_line_join_t line_join);
PLUTOVG_API void plutovg_canvas_set_miter_limit(plutovg_canvas_t* canvas, float miter_limit);

PLUTOVG_API float plutovg_canvas_get_line_width(const plutovg_canvas_t* canvas);
PLUTOVG_API plutovg_line_cap_t plutovg_canvas_get_line_cap(const plutovg_canvas_t* canvas);
PLUTOVG_API plutovg_line_join_t plutovg_canvas_get_line_join(const plutovg_canvas_t* canvas);
PLUTOVG_API float plutovg_canvas_get_miter_limit(const plutovg_canvas_t* canvas);

PLUTOVG_API void plutovg_canvas_set_dash(plutovg_canvas_t* canvas, float offset, const float* dashes, int ndashes);
PLUTOVG_API void plutovg_canvas_set_dash_offset(plutovg_canvas_t* canvas, float offset);
PLUTOVG_API void plutovg_canvas_set_dash_array(plutovg_canvas_t* canvas, const float* dashes, int ndashes);

PLUTOVG_API float plutovg_canvas_get_dash_offset(const plutovg_canvas_t* canvas);
PLUTOVG_API int plutovg_canvas_get_dash_array(const plutovg_canvas_t* canvas, const float** dashes);

/**
 * @brief Translates the current transformation matrix by offsets `tx` and `ty`.
 * @param canvas A pointer to the `plutovg_canvas_t` canvas.
 * @param tx The translation offset in the x-direction.
 * @param ty The translation offset in the y-direction.
 */
PLUTOVG_API void plutovg_canvas_translate(plutovg_canvas_t* canvas, float tx, float ty);

/**
 * @brief Scales the current transformation matrix by factors `sx` and `sy`.
 * @param canvas A pointer to the `plutovg_canvas_t` canvas.
 * @param sx The scaling factor in the x-direction.
 * @param sy The scaling factor in the y-direction.
 */
PLUTOVG_API void plutovg_canvas_scale(plutovg_canvas_t* canvas, float sx, float sy);

/**
 * @brief Shears the current transformation matrix by factors `shx` and `shy`.
 * @param canvas A pointer to the `plutovg_canvas_t` canvas.
 * @param shx The shearing factor in the x-direction.
 * @param shy The shearing factor in the y-direction.
 */
PLUTOVG_API void plutovg_canvas_shear(plutovg_canvas_t* canvas, float shx, float shy);

/**
 * @brief Rotates the current transformation matrix by the specified angle (in radians).
 * @param canvas A pointer to the `plutovg_canvas_t` canvas.
 * @param angle The rotation angle in radians.
 */
PLUTOVG_API void plutovg_canvas_rotate(plutovg_canvas_t* canvas, float angle);

/**
 * @brief Multiplies the current transformation matrix with the specified `matrix`.
 * @param canvas A pointer to the `plutovg_canvas_t` canvas.
 * @param matrix A pointer to the `plutovg_matrix_t` transformation matrix.
 */
PLUTOVG_API void plutovg_canvas_transform(plutovg_canvas_t* canvas, const plutovg_matrix_t* matrix);

/**
 * @brief Resets the current transformation matrix to the identity matrix.
 * @param canvas A pointer to the `plutovg_canvas_t` canvas.
 */
PLUTOVG_API void plutovg_canvas_reset_matrix(plutovg_canvas_t* canvas);

/**
 * @brief Resets the current transformation matrix to the specified `matrix`.
 * @param canvas A pointer to the `plutovg_canvas_t` canvas.
 * @param matrix A pointer to the `plutovg_matrix_t` transformation matrix.
 */
PLUTOVG_API void plutovg_canvas_set_matrix(plutovg_canvas_t* canvas, const plutovg_matrix_t* matrix);

/**
 * @brief Stores the current transformation matrix in `matrix`.
 * @param canvas A pointer to the `plutovg_canvas_t` canvas.
 * @param A pointer to the `plutovg_matrix_t` to store the matrix.
 */
PLUTOVG_API void plutovg_canvas_get_matrix(const plutovg_canvas_t* canvas, plutovg_matrix_t* matrix);

/**
 * @brief Transforms the point `(x, y)` using the current transformation matrix and stores the result in `(xx, yy)`.
 * @param canvas A pointer to the `plutovg_canvas_t` canvas.
 * @param x The x-coordinate of the point to transform.
 * @param y The y-coordinate of the point to transform.
 * @param xx A pointer to store the transformed x-coordinate.
 * @param yy A pointer to store the transformed y-coordinate.
 */
PLUTOVG_API void plutovg_canvas_map(const plutovg_canvas_t* canvas, float x, float y, float* xx, float* yy);

/**
 * @brief Transforms the `src` point using the current transformation matrix and stores the result in `dst`.
 * @param canvas A pointer to the `plutovg_canvas_t` canvas.
 * @param src A pointer to the `plutovg_point_t` point to transform.
 * @param dst A pointer to the `plutovg_point_t` to store the transformed point.
 */
PLUTOVG_API void plutovg_canvas_map_point(const plutovg_canvas_t* canvas, const plutovg_point_t* src, plutovg_point_t* dst);

/**
 * @brief Transforms the `src` rectangle using the current transformation matrix and stores the result in `dst`.
 * @param canvas A pointer to the `plutovg_canvas_t` canvas.
 * @param src A pointer to the `plutovg_rect_t` rectangle to transform.
 * @param dst A pointer to the `plutovg_rect_t` to store the transformed rectangle.
 */
PLUTOVG_API void plutovg_canvas_map_rect(const plutovg_canvas_t* canvas, const plutovg_rect_t* src, plutovg_rect_t* dst);

/**
 * @brief Moves the current point to a new position.
 *
 * Moves the current point to the specified coordinates without adding a line.
 * This operation is added to the current path. Equivalent to the SVG `M` command.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param x The x-coordinate of the new position.
 * @param y The y-coordinate of the new position.
 */
PLUTOVG_API void plutovg_canvas_move_to(plutovg_canvas_t* canvas, float x, float y);

/**
 * @brief Adds a straight line segment to the current path.
 *
 * Adds a straight line from the current point to the specified coordinates.
 * This segment is added to the current path. Equivalent to the SVG `L` command.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param x The x-coordinate of the end point of the line.
 * @param y The y-coordinate of the end point of the line.
 */
PLUTOVG_API void plutovg_canvas_line_to(plutovg_canvas_t* canvas, float x, float y);

/**
 * @brief Adds a quadratic Bézier curve to the current path.
 *
 * Adds a quadratic Bézier curve from the current point to the specified end point,
 * using the given control point. This curve is added to the current path. Equivalent to the SVG `Q` command.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param x1 The x-coordinate of the control point.
 * @param y1 The y-coordinate of the control point.
 * @param x2 The x-coordinate of the end point of the curve.
 * @param y2 The y-coordinate of the end point of the curve.
 */
PLUTOVG_API void plutovg_canvas_quad_to(plutovg_canvas_t* canvas, float x1, float y1, float x2, float y2);

/**
 * @brief Adds a cubic Bézier curve to the current path.
 *
 * Adds a cubic Bézier curve from the current point to the specified end point,
 * using the given control points. This curve is added to the current path. Equivalent to the SVG `C` command.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param x1 The x-coordinate of the first control point.
 * @param y1 The y-coordinate of the first control point.
 * @param x2 The x-coordinate of the second control point.
 * @param y2 The y-coordinate of the second control point.
 * @param x3 The x-coordinate of the end point of the curve.
 * @param y3 The y-coordinate of the end point of the curve.
 */
PLUTOVG_API void plutovg_canvas_cubic_to(plutovg_canvas_t* canvas, float x1, float y1, float x2, float y2, float x3, float y3);

/**
 * @brief Adds an elliptical arc to the current path.
 *
 * Adds an elliptical arc from the current point to the specified end point,
 * defined by radii, rotation angle, and flags for arc type and direction.
 * This arc segment is added to the current path. Equivalent to the SVG `A` command.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param rx The x-radius of the ellipse.
 * @param ry The y-radius of the ellipse.
 * @param angle The rotation angle of the ellipse in degrees.
 * @param large_arc_flag If true, add the large arc; otherwise, add the small arc.
 * @param sweep_flag If true, add the arc in the positive-angle direction; otherwise, in the negative-angle direction.
 * @param x The x-coordinate of the end point.
 * @param y The y-coordinate of the end point.
 */
PLUTOVG_API void plutovg_canvas_arc_to(plutovg_canvas_t* canvas, float rx, float ry, float angle, bool large_arc_flag, bool sweep_flag, float x, float y);

/**
 * @brief Adds a rectangle to the current path.
 *
 * Adds a rectangle with the specified position and dimensions to the current path.
 * Equivalent to the SVG `rect` command.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param x The x-coordinate of the rectangle's origin.
 * @param y The y-coordinate of the rectangle's origin.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 */
PLUTOVG_API void plutovg_canvas_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h);

/**
 * @brief Adds a rounded rectangle to the current path.
 *
 * Adds a rectangle with rounded corners defined by the specified position,
 * dimensions, and corner radii to the current path.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param x The x-coordinate of the rectangle's origin.
 * @param y The y-coordinate of the rectangle's origin.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param rx The x-radius of the corners.
 * @param ry The y-radius of the corners.
 */
PLUTOVG_API void plutovg_canvas_round_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h, float rx, float ry);

/**
 * @brief Adds an ellipse to the current path.
 *
 * Adds an ellipse centered at the specified coordinates with the given radii to the current path.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param cx The x-coordinate of the ellipse's center.
 * @param cy The y-coordinate of the ellipse's center.
 * @param rx The x-radius of the ellipse.
 * @param ry The y-radius of the ellipse.
 */
PLUTOVG_API void plutovg_canvas_ellipse(plutovg_canvas_t* canvas, float cx, float cy, float rx, float ry);

/**
 * @brief Adds a circle to the current path.
 *
 * Adds a circle centered at the specified coordinates with the given radius to the current path.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param cx The x-coordinate of the circle's center.
 * @param cy The y-coordinate of the circle's center.
 * @param r The radius of the circle.
 */
PLUTOVG_API void plutovg_canvas_circle(plutovg_canvas_t* canvas, float cx, float cy, float r);

/**
 * @brief Adds an arc to the current path.
 *
 * Adds an arc centered at the specified coordinates, with a given radius,
 * starting and ending at the specified angles. The direction of the arc is
 * determined by `ccw`. This arc segment is added to the current path.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param cx The x-coordinate of the arc's center.
 * @param cy The y-coordinate of the arc's center.
 * @param r The radius of the arc.
 * @param a0 The starting angle of the arc in radians.
 * @param a1 The ending angle of the arc in radians.
 * @param ccw If true, add the arc counter-clockwise; otherwise, clockwise.
 */
PLUTOVG_API void plutovg_canvas_arc(plutovg_canvas_t* canvas, float cx, float cy, float r, float a0, float a1, bool ccw);

/**
 * @brief Adds a path to the current path.
 *
 * Appends the elements of the specified path to the current path.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param path A pointer to the `plutovg_path_t` object to be added.
 */
PLUTOVG_API void plutovg_canvas_add_path(plutovg_canvas_t* canvas, const plutovg_path_t* path);

/**
 * @brief Starts a new path on the canvas.
 *
 * Begins a new path, clearing any existing path data. The new path starts with no commands.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 */
PLUTOVG_API void plutovg_canvas_new_path(plutovg_canvas_t* canvas);

/**
 * @brief Closes the current path.
 *
 * Closes the current path by adding a straight line back to the starting point.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 */
PLUTOVG_API void plutovg_canvas_close_path(plutovg_canvas_t* canvas);

/**
 * @brief Retrieves the current point of the canvas.
 *
 * Gets the coordinates of the current point in the canvas, which is the last point
 * added or moved to in the current path.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @param x Output parameter for the x-coordinate of the current point.
 * @param y Output parameter for the y-coordinate of the current point.
 */
PLUTOVG_API void plutovg_canvas_get_current_point(const plutovg_canvas_t* canvas, float* x, float* y);

/**
 * @brief Gets the current path from the canvas.
 *
 * Retrieves the path object representing the sequence of path commands added to the canvas.
 *
 * @param canvas A pointer to the `plutovg_canvas_t` object.
 * @return A pointer to the `plutovg_path_t` object representing the current path.
 */
PLUTOVG_API plutovg_path_t* plutovg_canvas_get_path(const plutovg_canvas_t* canvas);

PLUTOVG_API void plutovg_canvas_fill_extents(const plutovg_canvas_t* canvas, plutovg_rect_t* extents);
PLUTOVG_API void plutovg_canvas_stroke_extents(const plutovg_canvas_t* canvas, plutovg_rect_t* extents);
PLUTOVG_API void plutovg_canvas_clip_extents(const plutovg_canvas_t* canvas, plutovg_rect_t* extents);

PLUTOVG_API void plutovg_canvas_fill(plutovg_canvas_t* canvas);
PLUTOVG_API void plutovg_canvas_stroke(plutovg_canvas_t* canvas);
PLUTOVG_API void plutovg_canvas_clip(plutovg_canvas_t* canvas);
PLUTOVG_API void plutovg_canvas_paint(plutovg_canvas_t* canvas);

PLUTOVG_API void plutovg_canvas_fill_preserve(plutovg_canvas_t* canvas);
PLUTOVG_API void plutovg_canvas_stroke_preserve(plutovg_canvas_t* canvas);
PLUTOVG_API void plutovg_canvas_clip_preserve(plutovg_canvas_t* canvas);

PLUTOVG_API void plutovg_canvas_fill_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h);
PLUTOVG_API void plutovg_canvas_fill_path(plutovg_canvas_t* canvas, const plutovg_path_t* path);

PLUTOVG_API void plutovg_canvas_stroke_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h);
PLUTOVG_API void plutovg_canvas_stroke_path(plutovg_canvas_t* canvas, const plutovg_path_t* path);

PLUTOVG_API void plutovg_canvas_clip_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h);
PLUTOVG_API void plutovg_canvas_clip_path(plutovg_canvas_t* canvas, const plutovg_path_t* path);

PLUTOVG_API float plutovg_canvas_add_glyph(plutovg_canvas_t* canvas, plutovg_codepoint_t codepoint, float x, float y);
PLUTOVG_API float plutovg_canvas_add_text(plutovg_canvas_t* canvas, const void* text, int length, plutovg_text_encoding_t encoding, float x, float y);

PLUTOVG_API float plutovg_canvas_fill_text(plutovg_canvas_t* canvas, const void* text, int length, plutovg_text_encoding_t encoding, float x, float y);
PLUTOVG_API float plutovg_canvas_stroke_text(plutovg_canvas_t* canvas, const void* text, int length, plutovg_text_encoding_t encoding, float x, float y);
PLUTOVG_API float plutovg_canvas_clip_text(plutovg_canvas_t* canvas, const void* text, int length, plutovg_text_encoding_t encoding, float x, float y);

PLUTOVG_API void plutovg_canvas_font_metrics(plutovg_canvas_t* canvas, float* ascent, float* descent, float* line_gap, plutovg_rect_t* extents);
PLUTOVG_API void plutovg_canvas_glyph_metrics(plutovg_canvas_t* canvas, plutovg_codepoint_t codepoint, float* advance_width, float* left_side_bearing, plutovg_rect_t* extents);
PLUTOVG_API float plutovg_canvas_text_extents(plutovg_canvas_t* canvas, const void* text, int length, plutovg_text_encoding_t encoding, plutovg_rect_t* extents);

#ifdef __cplusplus
}
#endif

#endif // PLUTOVG_H
