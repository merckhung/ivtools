#version 450
// Animated blueprint backdrop. A deep-blue drafting sheet with a minor and a
// major grid; bright pulses run along the major lines (messages between
// objects) and the sheet glows faintly in the current tour's accent colour.
// Everything the Skia layer leaves transparent shows this.

layout(push_constant) uniform Params {
  float time;      // seconds
  float progress;  // 0..1 through all tours
  vec2 size;       // framebuffer size in pixels
  vec4 accent;     // rgb, intensity
} p;

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 out_color;

float hash(float v) { return fract(sin(v * 127.1) * 43758.5453); }

// Distance in pixels to the nearest line of a grid with the given pitch.
float grid(vec2 px, float pitch) {
  vec2 f = abs(fract(px / pitch) - 0.5) * pitch;
  return (pitch * 0.5) - max(f.x, f.y);
}

void main() {
  vec2 px = v_uv * p.size;
  float scale = p.size.y / 900.0;

  // Sheet: blueprint navy with a soft vignette.
  vec3 sheet = mix(vec3(0.016, 0.035, 0.070), vec3(0.020, 0.050, 0.095), v_uv.y);
  float vig = smoothstep(1.25, 0.25, length(v_uv - 0.5) * 1.6);
  vec3 color = sheet * (0.55 + 0.45 * vig);

  // Minor and major grid lines.
  float minor = smoothstep(1.0 * scale, 0.0, grid(px, 20.0 * scale));
  float major = smoothstep(1.3 * scale, 0.0, grid(px, 100.0 * scale));
  color += vec3(0.05, 0.10, 0.17) * minor * 0.35;
  color += vec3(0.07, 0.15, 0.24) * major * 0.8;

  // Pulses travelling along some major lines, alternately across and down.
  float pitch = 100.0 * scale;
  vec2 cell = floor(px / pitch + 0.5);
  vec2 d = abs(px - cell * pitch);
  float on_row = step(0.5, hash(cell.y + 3.0));
  float on_col = step(0.6, hash(cell.x + 11.0));
  float row_line = on_row * smoothstep(1.6 * scale, 0.0, d.y);
  float col_line = on_col * smoothstep(1.6 * scale, 0.0, d.x);
  float speed = 180.0 * scale;
  float pr = fract((px.x - p.time * speed * (0.6 + hash(cell.y))) / (700.0 * scale) + hash(cell.y + 5.0));
  float pc = fract((px.y - p.time * speed * (0.6 + hash(cell.x))) / (560.0 * scale) + hash(cell.x + 9.0));
  pr = smoothstep(0.0, 0.05, pr) * smoothstep(0.12, 0.05, pr);
  pc = smoothstep(0.0, 0.05, pc) * smoothstep(0.12, 0.05, pc);
  color += p.accent.rgb * (row_line * pr + col_line * pc) * 0.8 * p.accent.a;

  // Tour glow from the top-left, growing as the tours progress.
  float glow = exp(-length(v_uv - vec2(0.18, 0.1)) * 2.6);
  color += p.accent.rgb * glow * (0.05 + 0.07 * p.progress) * p.accent.a;

  out_color = vec4(color, 1.0);
}
