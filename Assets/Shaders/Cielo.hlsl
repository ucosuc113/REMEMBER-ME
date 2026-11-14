// El cielo de Hab2: un degradado con sol y nubes, dibujado por dentro de una
// esfera grande que rodea la habitacion. El motor no tiene skybox, y no hace
// falta uno: lo unico que se ve del cielo es lo que entra por la ventana alta,
// y una esfera con este shader lo resuelve sin tocar el motor.
//
// La direccion se saca de la camara (CameraWorldPosPad) y no de la normal de la
// esfera: asi el cielo no depende de donde este el centro de la esfera ni de su
// tamano, y el sol queda en el mismo sitio del cielo aunque la persona camine.
//
//   UserParams[0]  rgb color del horizonte
//   UserParams[1]  rgb color del cenit
//   UserParams[2]  rgb color de abajo (por si se ve por debajo del horizonte)
//   UserParams[3]  xyz la direccion en la que VIAJA la luz del sol (la del nodo
//                  Sol): el disco se dibuja en la direccion contraria
//   UserParams[4]  rgb color del sol
//   UserParams[5]  x cuanto cielo cubren las nubes (0..1)  y velocidad del viento
//
// No pasa por la luz de la escena a proposito: el cielo es la fuente, no algo
// iluminado. Se devuelve el color tal cual; el ajuste de color de la camara y
// el post-proceso lo alcanzan igual que a todo lo demas.

float Hash21(float2 p) {
    p = frac(p * float2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return frac(p.x * p.y);
}

float Ruido(float2 p) {
    float2 i = floor(p);
    float2 f = frac(p);
    float2 u = f * f * (3.0 - 2.0 * f);
    float a = Hash21(i);
    float b = Hash21(i + float2(1, 0));
    float c = Hash21(i + float2(0, 1));
    float d = Hash21(i + float2(1, 1));
    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

float Nubes(float2 p) {
    float v = 0.0;
    float amp = 0.5;
    [unroll] for (int i = 0; i < 5; ++i) {
        v += Ruido(p) * amp;
        p *= 2.03;
        amp *= 0.5;
    }
    return v;
}

float4 PSMain(PSInput input) : SV_TARGET {
    float3 d = normalize(input.worldPos - CameraWorldPosPad.xyz);
    float3 horizonte = UserParams[0].rgb;
    float3 cenit = UserParams[1].rgb;
    float3 abajo = UserParams[2].rgb;
    float3 haciaSol = normalize(-UserParams[3].xyz);
    float3 sol = UserParams[4].rgb;
    float cobertura = UserParams[5].x;
    float viento = UserParams[5].y;

    float3 col;
    if (d.y >= 0.0) {
        col = lerp(horizonte, cenit, pow(saturate(d.y), 0.55));
        // Las nubes, en un plano a cierta altura: proyectar la direccion sobre
        // el es lo que las achica y las junta hacia el horizonte.
        float2 p = d.xz / max(d.y, 0.08) * 1.6 + float2(Time.x * viento, Time.x * viento * 0.4);
        float n = Nubes(p);
        float c = smoothstep(1.0 - cobertura, 1.0 - cobertura + 0.25, n) * saturate(d.y * 6.0);
        float3 nube = lerp(float3(0.95, 0.95, 0.97), horizonte * 0.85, 0.3);
        col = lerp(col, nube, c * 0.85);
    } else {
        col = lerp(horizonte, abajo, saturate(-d.y * 4.0));
    }

    float s = saturate(dot(d, haciaSol));
    col += sol * (pow(s, 900.0) * 6.0 + pow(s, 24.0) * 0.35);
    return float4(col, 1.0);
}
