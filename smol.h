
/** @file		smol.h
 *	@brief		The core of the smol framework
 *	@details	
 *	@author		[Scott R Howell (Bombshell)](http://scottrhowell.com/) [\@GIT](https://github.com/thebombshell)
 *	@date		14th July 2018
 */
 
#ifndef SMOL_H
#define SMOL_H

#include "messenger.h"

/** @struct smol - An undefined struct used as an opaque pointer to a smol context 
 *	@details The **smol** struct is presented to the user as an opaque pointer as its actual contents are platform specific. The **smol** pointer can be allocated via [alloc_smol](@ref alloc_smol) and is then used to communicate with the platform, for needs such as creating graphical contexts, handling threads, accessing files and communicating over the network.
 */
typedef struct smol smol;

/**	@struct smol_worker - An undefined struct used as an opaque pointer to a smol worker
 *	@details The **smol_worker** struct is presented to the user as an opaque pointer as its actual contents are platform specific. The **smol_worker** is handled mostly by smol along with other platform specifics, but can be queried for its intended use. The **smol_worker**'s intended use is that of an EMCA worker, as such it communicates and is communicated with via a _post message_ and _handle message_ system. By default the smol context handles two workers; A helper worker, used for asynchronous data processing for things such as content loading and low priority sorting and simulations; And a logic worker, used for high priority simulation, user input handling, and as far as the game developer is concerned, the entry point of the program.
 */
typedef struct smol_worker smol_worker;

/** A struct containing names and values of various uniforms 
 */
typedef struct smol_uniform_object {
	
	/** */
	unsigned int count;
	unsigned int size;
	char** names;
	unsigned int* types;
	void** values;
} smol_uniform_object;

/** A struct containing information regarding a render pass
 */
typedef struct smol_pass {
	
	unsigned int name;
	int count;
	char** uniforms;
	unsigned int* types;
	unsigned int* locations;
	int* sizes;
} smol_pass;

/** A struct containing information about a stage of rendering
 */
typedef struct smol_stage {
	
	unsigned int pass_count;
	smol_pass** passes;
} smol_stage;

/** A struct containing information about a rendering path
 */
typedef struct smol_path {
	
	unsigned int pass_count;
	smol_pass** passes;
} smol_path;

/** A struct containing information about a render material
 */
typedef struct smol_material {
	
	smol_uniform_object* uniforms;
	smol_path* path;
} smol_material;

/** A struct containing information about a render mesh
 */
typedef struct smol_mesh {
	
	unsigned int names[2];
	unsigned int draw_type;
	unsigned int element_count;
} smol_mesh;

/** A struct containing information about a rendering draw call
 */
typedef struct smol_draw {
	
	smol_uniform_object* uniforms;
	smol_material* material;
	smol_mesh* mesh;
	void* next;
} smol_draw;

/** A struct containing information about a render view
 */
typedef struct smol_view {
	
	unsigned int stage_count;
	smol_stage** stages;
	smol_uniform_object* uniforms;
} smol_view;

/** A struct containign information about a renderable scene
 */
typedef struct smol_scene {
	
	unsigned int draw_count;
	smol_draw draw_head;
	smol_draw* draw_tail;
	unsigned int view_count;
	smol_view** views;
} smol_scene;

/* smol */

/**	Returns nonzero if t_smol is valid.
 *	@memberof	smol
 *	@param		t_smol - the smol context to be checked for validity
 *	@returns	nonzero if t_smol is valid
 */
int smol_is_valid(smol* t_smol);

/**	Returns nonzero if t_smol is running.
 *	@memberof	smol
 *	@param		t_smol - the smol context to be queried for state
 *	@returns	nonzero if t_smol is running
 */
int smol_is_running(smol* t_smol);

/**	Allocates and initializes a new smol context
 *	@memberof	smol
 *	@returns	a new smol context which should be checked for validity before use
 */
smol* alloc_smol();

/**	Finalizes and deallocates an existing smol context
 *	@memberof	smol
 *	@param		t_smol - the smol context to be freed
 */
void free_smol(smol* t_smol);

/**	Runs a smol context
 *	@memberof	smol
 *	@param		t_smol - the smol context to be run
 */
void smol_run(smol* t_smol);

/** Posts a message fitting the given description to the given smol contexts system thread
 *	@memberof	smol
 *	@param		t_smol - the smol context whose system to send the message to
 *	@param		t_is_persistent - if non-zero the given message will trigger each message loop until a dispose message is received (this should only be done with repeatable tasks, such as queuing a draw)
 *	@param		t_type - in identifier of the type of message being sent
 *	@param		t_data_size - the size of the data buffers
 *	@param		t_data_ptr - a pointer to the data buffers
 *	@returns	An immutable pointer to the completed message which is sent, intended for use with t_is_persistent, allowing the pointer to be sent later as a dispose message
 */
const smol_message* smol_post_system(smol* t_smol, int t_is_persistent, unsigned int t_type, unsigned int t_data_size, const void* t_data_ptr);

/** Posts a message fitting the given description to the given smol contexts logic thread
 *	@memberof	smol
 *	@param		t_smol - the smol context whose logic to send the message to
 *	@param		t_is_persistent - if non-zero the given message will trigger each message loop until a dispose message is received (this should only be done with repeatable tasks, such as queuing a draw)
 *	@param		t_type - in identifier of the type of message being sent
 *	@param		t_data_size - the size of the data buffers
 *	@param		t_data_ptr - a pointer to the data buffers
 *	@returns	An immutable pointer to the completed message which is sent, intended for use with t_is_persistent, allowing the pointer to be sent later as a dispose message
 */
const smol_message* smol_post_logic(smol* t_smol, int t_is_persistent, unsigned int t_type, unsigned int t_data_size, const void* t_data_ptr);

/** Posts a message fitting the given description to the given smol contexts helper thread
 *	@memberof	smol
 *	@param		t_smol - the smol context whose helper to send the message to
 *	@param		t_is_persistent - if non-zero the given message will trigger each message loop until a dispose message is received (this should only be done with repeatable tasks, such as queuing a draw)
 *	@param		t_type - in identifier of the type of message being sent
 *	@param		t_data_size - the size of the data buffers
 *	@param		t_data_ptr - a pointer to the data buffers
 *	@returns	An immutable pointer to the completed message which is sent, intended for use with t_is_persistent, allowing the pointer to be sent later as a dispose message
 */
const smol_message* smol_post_helper(smol* t_smol, int t_is_persistent, unsigned int t_type, unsigned int t_data_size, const void* t_data_ptr);


/** Swaps the backbuffer and forebuffer of a smol context
 *	@memberof	smol
 *	@param		t_smol - the smol context to swap buffers of
 */
void smol_swap_buffers(smol* t_smol);

/** Renders a smol context's scene
 *	@memberof	smol
 *	@param		t_smol - the smol context to render
 */
void smol_render(smol* t_smol);

/* smol_uniform_object */

/**	Allocates a new uniform object
 *	@memberof	smol_uniform_object
 *	@returns	a valid uniform object or 0 if error
 */
smol_uniform_object* smol_alloc_uniform();

/**	Finalizes and frees a uniform object
 *	@memberof	smol_uniform_object
 *	@param		t_uniform - the uniform object to free
 */
void smol_free_uniform(smol_uniform_object* t_uniform);

/**	Resizes a uniform objects buffers
 *	@memberof	smol_uniform_object
 *	@param		t_uniform - the uniform object to resize
 *	@param		t_size - the size to change to
 */
void smol_uniform_resize(smol_uniform_object* t_uniform, unsigned int t_size);

/**	Binds all uniforms of the given pass which are defined within the uniform object
 *	@memberof	smol_uniform_object
 *	@param		t_uniform - the uniform object to bind
 *	@param		t_pass - the pass to bind with
 */
void smol_uniform_bind(smol_uniform_object* t_uniform, smol_pass* t_pass);

/* smol_pass */

/**	Allocates a valid rendering pass
 *	@memberof	smol_pass
 *	@param		t_vertex_shader - the contents of a glsl vertex shader
 *	@param		t_fragment_shader - the contents of a glsl fragment shader
 *	@returns	a valid rendering pass or 0 if error
 */
smol_pass* smol_alloc_pass(const char* t_vertex_shader, const char* t_fragment_shader);

/** Finalizes and frees a rendering pass
 *	@memberof	smol_pass
 *	@param		t_pass - the rendering pass to free
 */
void smol_free_pass(smol_pass* t_pass);

/**	Binds a rendering pass to be used
 *	@memberof	smol_pass
 *	@param		t_pass - the rendering pass to bind
 */
void smol_pass_bind(smol_pass* t_pass);

/**	Returns the number of uniforms in a rendering pass
 *	@memberof	smol_pass
 *	@param		t_pass - the rendering pass to check for uniforms
 *	@returns	the number of uniforms
 */
unsigned int smol_pass_get_uniform_count(smol_pass* t_pass);

/**	Returns the name of a uniform in a rendering pass at a given index
 *	@memberof	smol_pass
 *	@param		t_pass - the rendering pass to check for uniforms
 *	@param		t_index - the index of the uniform
 *	@returns	the uniform at t_index
 */
const char* smol_pass_get_uniform(smol_pass* t_pass, unsigned int t_index);

/**	Binds a uniform value to a uniform at the given index
 *	@memberof	smol_pass
 *	@param		t_pass - the rendering pass to bind with
 *	@param		t_index - the index of the uniform
 *	@param		t_count - the count of values at t_value
 *	@param		t_value - a pointer to the raw data to set the uniform with
 */
void smol_pass_set_uniform(smol_pass* t_pass, unsigned int t_index, unsigned int t_count, void* t_value);

/* smol_stage */

/** Allocates and returns a valid rendering stage
 *	@memberof	smol_stage
 *	@param		t_pass_count - the number of passes the stage will handle
 *	@param		t_passes - an array of pointers to valid render passes
 *	@returns	A valid rendering stage
 */
smol_stage* smol_alloc_stage(unsigned int t_pass_count, smol_pass** t_passes);

/** Finalizes and frees a rendering stage
 *	@memberof	smol_stage
 *	@param		t_stage - the rendering stage to free
 */
void smol_free_stage(smol_stage* t_stage);

/** Returns the number of render passes the stage handles
 *	@memberof	smol_stage
 *	@param		t_stage - the rendering stage to get the pass count of
 *	@returns	the number of render passes the stage handles
 */
unsigned int smol_stage_get_pass_count(smol_stage* t_stage);

/** Returns a render pass of a given index
 *	@memberof	smol_stage
 *	@param		t_stage - the rendering stage to query for a pass
 *	@param		t_index - the index of the pass to return
 *	@returns	the render pass of the given index
 */
smol_pass* smol_stage_get_pass(smol_stage* t_stage, unsigned int t_index);

/* smol_path */

/**	Allocates and returns a render path
 *	@memberof	smol_path
 *	@param		t_pass_count - the number of passes the render path uses
 *	@param		t_passes - an array of pointers to valid render passes
 *	@returns	a valid render path
 */
smol_path* smol_alloc_path(unsigned int t_pass_count, smol_pass** t_passes);

/**	Finalizes and frees the given render path
 *	@memberof	smol_path
 *	@param		t_path - the render path to free
 */
void smol_free_path(smol_path* t_path);

/**	Returns the number of passes the render path uses
 *	@memberof	smol_path
 *	@param		t_path - the render path to query for pass count
 *	@returns	the number of passes the render path uses
 */
unsigned int smol_path_get_pass_count(smol_path* t_path);

/**	Returns the pass at a given index
 *	@memberof	smol_path
 *	@param		t_path - the render path to query for render paths
 *	@param		t_index - the index of the render pass
 *	@returns	the pass of the given index within the render pass
 */
smol_pass* smol_path_get_pass(smol_path* t_path, unsigned int t_index);

/* smol_material */

/**	Allocates and returns a valid render material
 *	@memberof	smol_material
 *	@param		t_path - the render path the material will use
 *	@param		t_uniforms - the uniform object the material will use
 *	@returns	a valid render material
 */
smol_material* smol_alloc_materail(smol_path* t_path, smol_uniform_object* t_uniforms);

/**	Finalizes and frees the given render material
 *	@memberof	smol_material
 *	@param		t_material - the render material to free
 */
void smol_free_material(smol_material* t_material);

/**	Returns the render path a material uses
 *	@memberof	smol_material
 *	@param		t_material - the material to query for render path
 *	@returns	the render path the material uses
 */
smol_path* smol_material_get_path(smol_material* t_material);

/**	Returns the uniform object the material uses
 *	@memberof	smol_material
 *	@param		t_material - the material to query for a uniform object
 *	@returns	the uniform object the material uses
 */
smol_uniform_object* smol_material_get_uniform_object(smol_material* t_material);

/* smol_mesh */

/** Allocates and returns a valid render mesh
 *	@memberof	smol_mesh
 *	@returns	a valid render mesh
 */
smol_mesh* smol_alloc_mesh();

/** Finalizes and frees a render mesh
 *	@memberof	smol_mesh
 *	@param		t_mesh - the render mesh to free
 */
void smol_free_mesh(smol_mesh* t_mesh);

/**	Binds a render mesh for drawing and or editing
 *	@memberof	smol_mesh
 *	@param		t_mesh - the render mesh to bind
 */
void smol_mesh_bind(smol_mesh* t_mesh);

/**	Draws a render mesh if bound, this does not bind the mesh
 *	@memberof	smol_mesh
 *	@param		t_mesh - the render mesh to draw
 */
void smol_mesh_draw(smol_mesh* t_mesh);

/* smol_draw */

/**
 *	@memberof	smol_draw
 */
smol_draw* smol_alloc_draw(smol_mesh* t_mesh, smol_material* t_material, smol_uniform_object* t_uniforms);

/**
 *	@memberof	smol_draw
 */
void smol_free_draw(smol_draw* t_draw);

/**
 *	@memberof	smol_draw
 */
smol_uniform_object* smol_draw_get_uniforms(smol_draw* t_draw);

/**
 *	@memberof	smol_draw
 */
smol_material* smol_draw_get_material(smol_draw* t_draw);

/**
 *	@memberof	smol_draw
 */
smol_mesh* smol_draw_get_mesh(smol_draw* t_draw);

/* smol_view */

smol_view* smol_alloc_view(unsigned int t_stage_count, smol_stage** t_stages, smol_uniform_object* t_uniforms);

void smol_free_view(smol_view* t_view);

unsigned int smol_scene_get_view_count(smol_scene* t_scene);

smol_view* smol_scene_get_view(smol_scene* t_scene, unsigned int t_index);

unsigned int smol_view_get_stage_count(smol_view* t_view);

smol_stage* smol_view_get_stage(smol_view* t_view, unsigned int t_index);

smol_uniform_object* smol_view_get_uniforms(smol_view* t_view);

void smol_view_query_visible_draws(smol_view* t_view, smol_scene* t_scene, unsigned int* t_count_out, smol_draw** t_draws_out);

/* smol_scene */

void smol_init_scene(smol_scene* t_scene);

void smol_final_scene(smol_scene* t_scene);

unsigned int smol_scene_get_draw_count(smol_scene* t_scene);

smol_draw* smol_scene_get_draw_head(smol_scene* t_scene);

#endif