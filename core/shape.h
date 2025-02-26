// -------------------------------------------------------------
//  Cubzh Core
//  shape.h
//  Created by Adrien Duermael on July 12, 2017.
// -------------------------------------------------------------

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>

#include "block.h"
#include "box.h"
#include "chunk.h"
#include "color_atlas.h"
#include "color_palette.h"
#include "config.h"
#include "flood_fill_lighting.h"
#include "index3d.h"
#include "map_string_float3.h"
#include "matrix4x4.h"
#include "octree.h"
#include "quaternion.h"
#include "ray.h"
#include "vertextbuffer.h"

typedef struct _RigidBody RigidBody;
typedef struct _Shape Shape;
typedef struct _int3 int3;
typedef struct _Scene Scene;
typedef struct _Transform Transform;
typedef struct _VertexBuffer VertexBuffer;
typedef struct _Chunk Chunk;

typedef struct _LoadShapeSettings {
    bool octree;
    bool lighting;
    bool isMutable;
} LoadShapeSettings;

#define POINT_OF_INTEREST_ORIGIN "origin"     // legacy
#define POINT_OF_INTEREST_HAND "Hand"         //
#define POINT_OF_INTEREST_HAT "Hat"           // POI in item for wearing it as a hat
#define POINT_OF_INTEREST_BACKPACK "Backpack" // POI in item for equipping it as a backpack

// A shape is a model made out of blocks. A list of chunks & an octree are used as
// datastructures respectively to allocate memory for rendering buffers and to
// perform physics queries.
// TODO: currently (feb. 2023), all shapes have an octree and block data is duplicated
//  in octree & chunks, ideally, we need to refacto the writing of VBs so that chunks
//  can be removed and VB writing uses octree underlying data
//
// Memory allocation for rendering buffers (loosely called here vertex buffers) is meant
// to minimize memory usage and maximize buffer occupancy in order to draw the shape
// in a minimum number of drawcalls. The initial buffer is relatively small, and each
// subsequent buffer is allocated on-demand with increased capacity, to account for the
// common case of a scene filled with many small shapes.

/// ShapeId type
typedef uint16_t ShapeId;

/// Shape draw mode
typedef uint8_t ShapeDrawMode;
#define SHAPE_DRAWMODE_DEFAULT 0
#define SHAPE_DRAWMODE_ALL_TRANSPARENT 1
#define SHAPE_DRAWMODE_HIGHLIGHT 2
#define SHAPE_DRAWMODE_GREY 4
#define SHAPE_DRAWMODE_GRID 8

/// Initializes mutex for shape thread safety.
/// This is to be called before using the shape API.
/// This is optional. If not called, the shape API is not thread-safe.
void shape_initThreadSafety(void);

// Creates an empty Shape
Shape *shape_make(void);
// Creates a copy of the given shape
Shape *shape_make_copy(Shape *origin);
// Creates a shape without octree, with a known size
Shape *shape_make_with_size(const SHAPE_SIZE_INT_T width,
                            const SHAPE_SIZE_INT_T height,
                            const SHAPE_SIZE_INT_T depth,
                            const bool isMutable);
// Creates a shape with octree i.e. allowing per-block physics
Shape *shape_make_with_octree(const SHAPE_SIZE_INT_T width,
                              const SHAPE_SIZE_INT_T height,
                              const SHAPE_SIZE_INT_T depth,
                              const bool isMutable);

VertexBuffer *shape_add_vertex_buffer(Shape *shape, bool transparency);

/// Returns false if retain fails
bool shape_retain(Shape *const shape);

/// Convenience function to release the shape's Transform
/// /!\ `transform_release` is the function responsible for freeing resources
void shape_release(Shape *const shape);
uint16_t shape_retain_count(const Shape *const s);
/// /!\ Only called by `transform_release` to free shape-specific resources
void shape_free(Shape *const shape);

ShapeId shape_get_id(const Shape *shape);

// removes all blocks from shape and resets its transform(s)
void shape_flush(Shape *shape);

// access palette reference to get or set the colors
ColorPalette *shape_get_palette(const Shape *shape);
void shape_set_palette(Shape *shape, ColorPalette *palette);

/// Gets the block in model OR transactions
const Block *shape_get_block(const Shape *const shape,
                             SHAPE_COORDS_INT_T x,
                             SHAPE_COORDS_INT_T y,
                             SHAPE_COORDS_INT_T z,
                             const bool luaCoords);
/// Gets the block in model at the time of calling
Block *shape_get_block_immediate(const Shape *const shape,
                                 SHAPE_COORDS_INT_T x,
                                 SHAPE_COORDS_INT_T y,
                                 SHAPE_COORDS_INT_T z,
                                 const bool luaCoords);

/// Returns whether the block is considered added.
/// (a block is not added if it is out of bounds of a fixed size shape, or if
/// the shape already has a block at the given coordinates)
///
/// This function resizes the shape if needed.
bool shape_add_block_from_lua(Shape *const shape,
                              Scene *scene,
                              const SHAPE_COLOR_INDEX_INT_T colorIndex,
                              const SHAPE_COORDS_INT_T luaX,
                              const SHAPE_COORDS_INT_T luaY,
                              const SHAPE_COORDS_INT_T luaZ);

/// Removes block from Shape, using the automatic transaction system.
/// Returns whether the block has been removed.
bool shape_remove_block_from_lua(Shape *const shape,
                                 Scene *scene,
                                 const SHAPE_COORDS_INT_T luaX,
                                 const SHAPE_COORDS_INT_T luaY,
                                 const SHAPE_COORDS_INT_T luaZ);

/// Replaces block from Shape, using the automatic transaction system.
/// Returns whether the block has been replaced.
bool shape_replace_block_from_lua(Shape *const shape,
                                  const SHAPE_COLOR_INDEX_INT_T newColorIndex,
                                  const SHAPE_COORDS_INT_T luaX,
                                  const SHAPE_COORDS_INT_T luaY,
                                  const SHAPE_COORDS_INT_T luaZ);

///
void shape_apply_current_transaction(Shape *const shape, bool keepPending);

/// @param useDefaultColor will translate a default color into shape palette
bool shape_add_block_with_color(Shape *shape,
                                const SHAPE_COLOR_INDEX_INT_T colorIndex,
                                SHAPE_COORDS_INT_T x,
                                SHAPE_COORDS_INT_T y,
                                SHAPE_COORDS_INT_T z,
                                const bool resizeIfNeeded,
                                const bool applyOffset,
                                bool useDefaultColor);

// removes block, returns true if the block has been removed, false otherwise
bool shape_remove_block(Shape *shape,
                        SHAPE_COORDS_INT_T x,
                        SHAPE_COORDS_INT_T y,
                        SHAPE_COORDS_INT_T z,
                        Block **blockBefore, // this is never used
                        const bool applyOffset,
                        const bool shrinkBox);

// paints block, returns 1 if the block has been painted, 0 otherwise
bool shape_paint_block(Shape *shape,
                       const SHAPE_COLOR_INDEX_INT_T colorIndex,
                       SHAPE_COORDS_INT_T x,
                       SHAPE_COORDS_INT_T y,
                       SHAPE_COORDS_INT_T z,
                       Block **blockBefore,
                       Block **blockAfter,
                       const bool applyOffset);

void shape_get_chunk_and_position_within(const Shape *shape,
                                         const int3 *pos,
                                         Chunk **chunk,
                                         int3 *chunk_pos,
                                         int3 *pos_in_chunk);

void shape_get_bounding_box_size(const Shape *shape, int3 *size);
void shape_get_allocated_size(const Shape *shape, int3 *size);
bool shape_is_within_allocated_bounds(const Shape *shape,
                                      const SHAPE_COORDS_INT_T x,
                                      const SHAPE_COORDS_INT_T y,
                                      const SHAPE_COORDS_INT_T z);

// converts given box to a world axis-aligned box relative to shape
void shape_box_to_aabox(const Shape *s, const Box *box, Box *aabox, bool isCollider);

// a bounding box is the smallest box containing all shape's blocks, it is axis-aligned and
// therefore is dependant on which space we express it in,
// (1) in model space, ie. "block coordinates", the AABB itself w/o transformation
const Box *shape_get_model_aabb(const Shape *s);
// (2) in local space, ie. the model AABB w/ local transformations applied (on-demand)
void shape_get_local_aabb(const Shape *s, Box *box);
// (3) in world space, ie. the model AABB w/ world transformations applied (cached)
void shape_get_world_aabb(Shape *s, Box *box);

// iterates over chunks and blocks to obtain size and origin
bool shape_compute_size_and_origin(const Shape *shape,
                                   SHAPE_SIZE_INT_T *size_x,
                                   SHAPE_SIZE_INT_T *size_y,
                                   SHAPE_SIZE_INT_T *size_z,
                                   SHAPE_COORDS_INT_T *origin_x,
                                   SHAPE_COORDS_INT_T *origin_y,
                                   SHAPE_COORDS_INT_T *origin_z);
// recomputes box from scratch, necessary after block removal
void shape_shrink_box(Shape *shape, bool forceColliderResize);
// expand box to include given coordinates
void shape_expand_box(Shape *s,
                      const SHAPE_COORDS_INT_T x,
                      const SHAPE_COORDS_INT_T y,
                      const SHAPE_COORDS_INT_T z);

/// Increases allocated size and offsets all shape data (blocks, POIs, pivot, baked lighting)
/// to make space around existing blocks, if needed. A shape w/o allocated size do not need this,
/// i.e. a shape with no lighting, no octree, only blocks in chunks
void shape_make_space_for_block(Shape *shape,
                                SHAPE_COORDS_INT_T x,
                                SHAPE_COORDS_INT_T y,
                                SHAPE_COORDS_INT_T z,
                                const bool applyOffset);
void shape_make_space(Shape *const shape,
                      SHAPE_COORDS_INT_T minX,
                      SHAPE_COORDS_INT_T minY,
                      SHAPE_COORDS_INT_T minZ,
                      SHAPE_COORDS_INT_T maxX,
                      SHAPE_COORDS_INT_T maxY,
                      SHAPE_COORDS_INT_T maxZ,
                      const bool applyOffset);

/// Updates a shape's vertex buffers.
void shape_refresh_vertices(Shape *shape);
void shape_refresh_all_vertices(Shape *s);

VertexBuffer *shape_get_first_vertex_buffer(const Shape *shape, bool transparent);

// it's caller responsability to release created Index3DIterator
Index3DIterator *shape_new_chunk_iterator(const Shape *shape);

size_t shape_get_nb_chunks(const Shape *shape);
size_t shape_get_nb_blocks(const Shape *shape);

const Octree *shape_get_octree(const Shape *shape);

void shape_log_vertex_buffers(const Shape *shape, bool dirtyOnly, bool transparent);

void shape_set_model_locked(Shape *s, bool toggle);
bool shape_is_model_locked(Shape *s);

void shape_set_fullname(Shape *s, const char *fullname); // copies fullname
const char *shape_get_fullname(const Shape *s);

void shape_set_color_palette_atlas(Shape *s, ColorAtlas *ca);

/// This flag is only used to check from VX whether or not a coder can call MutableShape functions
bool shape_is_lua_mutable(Shape *s);
void shape_set_lua_mutable(Shape *s, const bool value);

// MARK: - Transform -

/// optional pivot
/// notes:
/// - while the pivot point is eg. (2, 4, 3), the transformation required is a (-2, -4, -3)
/// translation, thus shape_get_pivot does not return the actual translation, but a negated value of
/// it to avoid confusion outside of Shape
/// - internal pivot is here to offset model origin to change the way transformations are applied to
/// the shape
/// - for a lua Shape, it is set to its geometric center by default at it is what most user would
/// expect
/// - the effect of internal pivot can entirely be reproduced by using a separate Transform or
/// Object in lua w/ a local position
void shape_set_pivot(Shape *s, const float x, const float y, const float z, bool removeOffset);
float3 shape_get_pivot(const Shape *s, bool applyOffset);
void shape_reset_pivot_to_center(Shape *s);

float3 shape_block_to_local(const Shape *s, const float x, const float y, const float z);
float3 shape_block_to_world(const Shape *s, const float x, const float y, const float z);
float3 shape_local_to_block(const Shape *s, const float x, const float y, const float z);
float3 shape_world_to_block(const Shape *s, const float x, const float y, const float z);
void shape_block_lua_to_internal(const Shape *s,
                                 SHAPE_COORDS_INT_T *x,
                                 SHAPE_COORDS_INT_T *y,
                                 SHAPE_COORDS_INT_T *z);
void shape_block_internal_to_lua(const Shape *s,
                                 SHAPE_COORDS_INT_T *x,
                                 SHAPE_COORDS_INT_T *y,
                                 SHAPE_COORDS_INT_T *z);
void shape_block_lua_to_internal_float(const Shape *s, float *x, float *y, float *z);
void shape_block_internal_to_lua_float(const Shape *s, float *x, float *y, float *z);

/// translation
void shape_set_position(Shape *s, const float x, const float y, const float z);
void shape_set_local_position(Shape *s, const float x, const float y, const float z);
const float3 *shape_get_position(const Shape *s);
const float3 *shape_get_local_position(const Shape *s);
const float3 *shape_get_model_origin(const Shape *s);

/// rotation
void shape_set_rotation(Shape *s, Quaternion *q);
void shape_set_rotation_euler(Shape *s, const float x, const float y, const float z);
void shape_set_local_rotation(Shape *s, Quaternion *q);
void shape_set_local_rotation_euler(Shape *s, const float x, const float y, const float z);
Quaternion *shape_get_rotation(const Shape *s);
void shape_get_rotation_euler(const Shape *s, float3 *euler);
Quaternion *shape_get_local_rotation(const Shape *s);
void shape_get_local_rotation_euler(const Shape *s, float3 *euler);

/// scale
void shape_set_local_scale(Shape *s, const float x, const float y, const float z);
const float3 *shape_get_local_scale(const Shape *s);
void shape_get_lossy_scale(const Shape *s, float3 *scale);

/// matrices
const Matrix4x4 *shape_get_model_matrix(const Shape *s);

bool shape_set_parent(Shape *s, Transform *parent, const bool keepWorld);
/// /!\ shape_remove_parent is INTERNAL use only,
/// for external usage i.e. for shapes that exist as a lua Shape, transform removal needs to be
/// registered in scene, see lua_object for examples
bool shape_remove_parent(Shape *s, const bool keepWorld);
Transform *shape_get_root_transform(const Shape *s);
Transform *shape_get_pivot_transform(const Shape *s); // corresponds to shape model origin
void shape_move_children(Shape *from, Shape *to, const bool keepWorld);
uint32_t shape_count_shape_descendants(const Shape *s);
DoublyLinkedListNode *shape_get_transform_children_iterator(const Shape *s);

// MARK: - Physics -

RigidBody *shape_get_rigidbody(const Shape *s);
uint8_t shape_get_collision_groups(const Shape *s);
bool shape_ensure_rigidbody(Shape *s,
                            const uint8_t groups,
                            const uint8_t collidesWith,
                            RigidBody **out);
void shape_fit_collider_to_bounding_box(const Shape *s);
const Box *shape_get_local_collider(const Shape *s);
void shape_compute_world_collider(const Shape *s, Box *box);

/// @param s shape model used as obstacle against a moving object
/// @param modelBox moving object collider aligned with shape model space
/// @param modelVector moving object velocity vector in shape model space
/// @param epsilon collision tolerance in shape model space
/// @param withReplacement typically true if used for simulation, false if used for cast/overlap
/// @param normal axis where the first collision will occur
/// @param extraReplacement filled only if PHYSICS_EXTRA_REPLACEMENTS is enabled
/// @param block ptr to first hit block, convenient to grab here
/// @param blockCoords coordinates of block param
float shape_box_swept(const Shape *s,
                      const Box *modelBox,
                      const float3 *modelVector,
                      const float3 *epsilon,
                      const bool withReplacement,
                      float3 *normal,
                      float3 *extraReplacement,
                      Block **block,
                      SHAPE_COORDS_INT3_T *blockCoords);

/// Casts a world ray against given shape. World distance, local impact, block & block octree
/// coordinates can be returned through pointer parameters
/// @return true if a block is touched
bool shape_ray_cast(const Shape *s,
                    const Ray *worldRay,
                    float *worldDistance,
                    float3 *localImpact,
                    Block **block,
                    SHAPE_COORDS_INT3_T *coords);
bool shape_point_overlap(const Shape *s, const float3 *world);
/// Overlaps a box in shape's model space against its blocks
/// @return true if there is an overlap
bool shape_box_overlap(const Shape *s, const Box *modelBox, Box *out);

// MARK: - Graphics -

bool shape_is_hidden(Shape *s);

void shape_set_draw_mode(Shape *s, ShapeDrawMode m);
ShapeDrawMode shape_get_draw_mode(const Shape *s);

void shape_set_inner_transparent_faces(Shape *s, const bool toggle);
bool shape_draw_inner_transparent_faces(const Shape *s);

void shape_set_shadow(Shape *s, const bool toggle);
bool shape_has_shadow(const Shape *s);

void shape_set_unlit(Shape *s, const bool value);
bool shape_is_unlit(const Shape *s);

void shape_set_layers(Shape *s, const uint8_t value);
uint8_t shape_get_layers(const Shape *s);

// MARK: -

void shape_debug_points_of_interest(const Shape *s);

MapStringFloat3Iterator *shape_get_poi_iterator(const Shape *s);
const float3 *shape_get_point_of_interest(const Shape *s, const char *key);
void shape_set_point_of_interest(Shape *s, const char *key, const float3 *f3);

MapStringFloat3Iterator *shape_get_point_rotation_iterator(const Shape *s);
void shape_set_point_rotation(Shape *s, const char *key, const float3 *f3);
const float3 *shape_get_point_rotation(const Shape *s, const char *key);

void shape_remove_point(Shape *s, const char *key);

// MARK: - Baked lighting -

/// Shape lighting may be initialized from baked lighting, or by calling this function directly.
/// If lighting was already computed from either way, this function will not overwrite unless
/// overwrite=true.
/// Calling this function will enable the use of baked lighting for the shape i.e. adding or
/// removing blocks will now update baked lighting
void shape_compute_baked_lighting(Shape *s, bool overwrite);

bool shape_has_baked_lighting_data(const Shape *s);
const VERTEX_LIGHT_STRUCT_T *shape_get_lighting_data(const Shape *s);
void shape_set_lighting_data(Shape *s, VERTEX_LIGHT_STRUCT_T *d);
VERTEX_LIGHT_STRUCT_T shape_get_light_without_checking(const Shape *s,
                                                       SHAPE_COORDS_INT_T x,
                                                       SHAPE_COORDS_INT_T y,
                                                       SHAPE_COORDS_INT_T z);
void shape_set_light(Shape *s,
                     SHAPE_COORDS_INT_T x,
                     SHAPE_COORDS_INT_T y,
                     SHAPE_COORDS_INT_T z,
                     VERTEX_LIGHT_STRUCT_T light);
// helper function that returns light or default light if pos out of bounds or if isDefault
VERTEX_LIGHT_STRUCT_T shape_get_light_or_default(Shape *s,
                                                 SHAPE_COORDS_INT_T x,
                                                 SHAPE_COORDS_INT_T y,
                                                 SHAPE_COORDS_INT_T z,
                                                 bool isDefault);

/// Block removal may open up sunlight or emission propagation, and/or remove emission sources
void shape_compute_baked_lighting_removed_block(Shape *s,
                                                SHAPE_COORDS_INT_T x,
                                                SHAPE_COORDS_INT_T y,
                                                SHAPE_COORDS_INT_T z,
                                                SHAPE_COLOR_INDEX_INT_T blockID);

/// Block addition may shut sunlight or emission propagation, and/or add emission sources
void shape_compute_baked_lighting_added_block(Shape *s,
                                              SHAPE_COORDS_INT_T x,
                                              SHAPE_COORDS_INT_T y,
                                              SHAPE_COORDS_INT_T z,
                                              SHAPE_COLOR_INDEX_INT_T blockID);

/// Block replacement may remove or replace emission sources
void shape_compute_baked_lighting_replaced_block(Shape *s,
                                                 SHAPE_COORDS_INT_T x,
                                                 SHAPE_COORDS_INT_T y,
                                                 SHAPE_COORDS_INT_T z,
                                                 SHAPE_COLOR_INDEX_INT_T blockID,
                                                 bool applyOffset);

uint64_t shape_get_baked_lighting_hash(const Shape *s);

// MARK: - History -

void shape_history_setEnabled(Shape *s, const bool enable);
bool shape_history_getEnabled(Shape *s);
void shape_history_setKeepTransactionPending(Shape *s, const bool b);
bool shape_history_getKeepTransactionPending(Shape *s);
bool shape_history_canUndo(const Shape *const s);
bool shape_history_canRedo(const Shape *const s);
void shape_history_undo(Shape *const s);
void shape_history_redo(Shape *const s);

// MARK: - Weak ptr -

Weakptr *shape_get_weakptr(Shape *s);
Weakptr *shape_get_and_retain_weakptr(Shape *s);

// MARK: -

void shape_enableAnimations(Shape *const s);
void shape_disableAnimations(Shape *const s);
bool shape_getIgnoreAnimations(Shape *const s);

#ifdef __cplusplus
} // extern "C"
#endif
