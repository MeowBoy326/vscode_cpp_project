/*********************************************************************************************************************/
/*                                                  /===-_---~~~~~~~~~------____                                     */
/*                                                 |===-~___                _,-'                                     */
/*                  -==\\                         `//~\\   ~~~~`---.___.-~~                                          */
/*              ______-==|                         | |  \\           _-~`                                            */
/*        __--~~~  ,-/-==\\                        | |   `\        ,'                                                */
/*     _-~       /'    |  \\                      / /      \      /                                                  */
/*   .'        /       |   \\                   /' /        \   /'                                                   */
/*  /  ____  /         |    \`\.__/-~~ ~ \ _ _/'  /          \/'                                                     */
/* /-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`                                                      */
/*                   \_|      /        _)   ;  ),   __--~~                                                           */
/*                     '~~--_/      _-~/-  / \   '-~ \                                                               */
/*                    {\__--_/}    / \\_>- )<__\      \                                                              */
/*                    /'   (_/  _-~  | |__>--<__|      |                                                             */
/*                   |0  0 _/) )-~     | |__>--<__|     |                                                            */
/*                   / /~ ,_/       / /__>---<__/      |                                                             */
/*                  o o _//        /-~_>---<__-~      /                                                              */
/*                  (^(~          /~_>---<__-      _-~                                                               */
/*                 ,/|           /__>--<__/     _-~                                                                  */
/*              ,//('(          |__>--<__|     /                  .----_                                             */
/*             ( ( '))          |__>--<__|    |                 /' _---_~\                                           */
/*          `-)) )) (           |__>--<__|    |               /'  /     ~\`\                                         */
/*         ,/,'//( (             \__>--<__\    \            /'  //        ||                                         */
/*       ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'                                          */
/*     `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/                                                     */
/*   ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~                                                       */
/*    ;'( ')/ ,)(                              ~~~~~~~~~~                                                            */
/*   ' ') '( (/                                                                                                      */
/*     '   '  `                                                                                                      */
/*********************************************************************************************************************/
#ifndef _PROBE_H_
#define _PROBE_H_

#include <linalgb.h>

struct probe {
    unsigned int cm;
    unsigned int irr_diffuse_cm;
    unsigned int prefiltered_cm;
};

struct probe_rndr {
    struct {
        unsigned int fb;
        unsigned int depth_rb;
    } glh;
    mat4 fproj;
    struct {
        int prev_vp[4];
        unsigned int prev_fb;
    } rs;
};

struct probe_vis {
    unsigned int vao, vbo, ebo;
    unsigned int num_indices;
    unsigned int shdr;
};

/* Probe interface */
void probe_init(struct probe* p);
void probe_destroy(struct probe* p);

/* Probe renderer interface */
void probe_rndr_init(struct probe_rndr* pr);
void probe_rndr_destroy(struct probe_rndr* pr);
void probe_render_begin(struct probe_rndr* pr);
void probe_render_side_begin(struct probe_rndr* pr, unsigned int side, struct probe* p, vec3 pos, mat4* view, mat4* proj);
void probe_render_side_end(struct probe_rndr* pr, unsigned int side);
void probe_render_end(struct probe_rndr* pr);

/* Probe processing interface */
void probe_extract_shcoeffs(double sh_coef[25][3], struct probe* p);
void probe_preprocess(struct probe* p, unsigned int irr_conv_shdr, unsigned int prefilt_shdr);

/* Probe visualize */
void probe_vis_render(struct probe*, vec3 probe_pos, unsigned int vis_shdr, mat4 view, mat4 proj, int mode);

/* Misc */
unsigned int brdf_lut_generate(unsigned int brdf_lut_shdr);

/* Convenience macros */
#define probe_render_faces(pr, p, pos, fview, fproj) \
    for (int _break = (probe_render_begin(pr), 1); _break; probe_render_end(pr), _break = 0) \
        for (unsigned int side = 0; (side < 6 && (probe_render_side_begin(pr, side, p, pos, &fview, &fproj), 1)); \
                (probe_render_side_end(pr, side), ++side))

#endif /* ! _PROBE_H_ */
