#define FCOLOR(r, g, b, a)                                                 \
  Color {                                                                  \
    (unsigned char)((r)*255), (unsigned char)((g)*255),                    \
    (unsigned char)((b)*255), (unsigned char)((a)*255)                     \
  }

#define VEC3_TO_COLOR(v) FCOLOR(v.x, v.y, v.z, 1.0f)
#define VEC4_TO_COLOR(v) FCOLOR((v).x, (v).y, (v).z, (v).w)

#define COLOR_TO_VEC4(c) Vector4{c.r/255.0f, c.g/255.0f, c.b/255.0f, c.a/255.0f};
