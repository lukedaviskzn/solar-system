#version 410 core

#define PI 3.1415926538

out vec4 FragColor;

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_world_position;
layout (location = 2) in vec3 v_normal;

uniform bool has_night;
uniform int lighting;

uniform sampler2D albedo;
uniform sampler2D albedo_night;

void main() {
   float lat = (atan(v_position.x, v_position.z) + PI) / 2.0 / PI;
   float long = (acos(v_position.y) + PI) / PI;

   vec3 sun_dir = -normalize(v_world_position);

   vec4 colour;
   
   // unlit sky or sun corona
   if (lighting == 0) {
      colour = texture(albedo, vec2(lat, long));
   } else if (lighting == 1) { // normal planets
      if (has_night) {
         float brightness = smoothstep(0.25, 0.75, dot(v_normal, sun_dir) / 2.0 + 0.5);
         
         colour = vec4(mix(texture(albedo_night, vec2(lat, long)).rgb, texture(albedo, vec2(lat, long)).rgb, brightness), 1.0);
      } else {
         float ambient = 0.05;

         float brightness = smoothstep(0.25, 0.75, dot(v_normal, sun_dir) / 2.0 + 0.5) * (1.0 - ambient) + ambient;

         colour = vec4(texture(albedo, vec2(lat, long)).rgb * brightness, 1.0);
      }
   } else if (lighting == 2) { // clouds
      float ambient = 0.05;

      float brightness = smoothstep(0.25, 0.75, dot(v_normal, sun_dir) / 2.0 + 0.5) * (1.0 - ambient) + ambient;

      vec4 base = texture(albedo, vec2(lat, long));

      colour = vec4(1.0, 1.0, 1.0, base.r * base.a * brightness);
   } else if (lighting == 3) { // atmosphere shells
      float ambient = 0.05;

      float brightness = smoothstep(0.25, 0.75, dot(v_normal, sun_dir) / 2.0 + 0.5) * (1.0 - ambient) + ambient;

      vec4 base = texture(albedo, vec2(lat, long));

      colour = base * brightness;
   } else if (lighting == 4) { // sun
      colour = texture(albedo, vec2(lat, long)) + vec4(0.75); // add emissive
   }
   
   FragColor = colour;
}