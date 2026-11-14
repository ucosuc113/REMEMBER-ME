// El aspecto de pantalla de REMEMBER ME: vineta, grano, aberracion de color,
// y el titulo que cambia de tinta segun el fondo que tiene detras.
//
// Es un recuerdo, no una camara: la imagen se oscurece hacia los bordes como
// cuando se mira algo de memoria, y el grano y la aberracion le quitan la
// limpieza de render que delataria que es un juego. Los tres son suaves a
// proposito: el reloj tiene que seguir leyendose "6:67" a primera vista.
//
//   UserParams[0]  x fuerza de la vineta (0..1)   y radio donde empieza (0..1)
//                  z grano (0..0,2)                w aberracion en el borde (texeles)
//   UserParams[1]  x velocidad del grano (cuadros por segundo)
//                  y por debajo de que brillo un fondo cuenta como OSCURO (0,35)
//                  z brillo de la tinta clara (0,92)
//   UserParams[2]  la zona del titulo, en pixeles de la UI (alto de referencia
//                  720, origen en el CENTRO, Y hacia abajo): x0, y0, x1, y1
//   UserParams[3]  la zona de JUGAR, igual. Son dos y no un rectangulo que
//                  cubra las dos cosas: entre ellas pasan las almohadas, y
//                  sus sombras junto al gris de la funda se aclaraban.
//
// LA TINTA DEL TITULO
//
// El titulo es negro sobre la pared clara. Donde algo oscuro le pasa por
// detras (una pared en sombra, una franja en diagonal), el negro se pierde.
// Una UI no sabe que tiene detras, pero este pase si: le llega la imagen ya
// compuesta. Para cada pixel de la zona se busca el fondo MAS CERCANO que sea
// mas claro que el (anillos de radio creciente): si el pixel es letra, ese
// anillo cae en el fondo de al lado. Si el fondo es oscuro, la letra pasa a la
// tinta clara, en proporcion a cuanto mas oscura que el fondo era - asi el
// borde suavizado de la letra queda suavizado, no dentado.
//
// Por que el anillo mas CERCANO y no el maximo de todos: una letra que cruza la
// diagonal tiene fondo claro a un lado y oscuro al otro. Con el maximo, el lado
// oscuro "veria" la pared clara de enfrente y se quedaria negro en una franja
// del ancho del radio. Con el mas cercano, cada trozo de letra mira SU fondo y
// el cambio de tinta cae justo en la raya.
//
// Por que solo en una zona: fuera del titulo, la misma regla aclararia
// cualquier sombra que este junto a algo menos oscuro (los pliegues de la
// cama, la pantalla negra del reloj junto a sus numeros). La zona va en
// pixeles de la UI con origen en el centro - la misma cuenta que usa la UI -
// para que siga al titulo en cualquier proporcion de pantalla.

float Hash(float2 p) {
    p = frac(p * float2(443.897, 441.423));
    p += dot(p, p.yx + 19.19);
    return frac((p.x + p.y) * p.x);
}

float Luma(float3 c) { return dot(c, float3(0.299, 0.587, 0.114)); }

float3 TitleInk(float2 uv, float3 col, float2 texel, float uiScale) {
    const float darkBelow = UserParams[1].y;
    const float lightInk = UserParams[1].z;
    float lum = Luma(col);
    // Ocho direcciones, anillos de 2 en 2 px de UI hasta 14: el trazo del
    // titulo mide unos 14 de ancho, asi que desde su centro el fondo esta a 7.
    static const float2 dirs[8] = {
        float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1),
        float2(0.7071, 0.7071), float2(-0.7071, 0.7071), float2(0.7071, -0.7071), float2(-0.7071, -0.7071)};
    // El primer anillo mas claro suele caer en el BORDE SUAVIZADO de la propia
    // letra (un gris a medio camino), no en el fondo: tomarlo convertia el
    // interior de cada trazo en "letra sobre fondo gris oscuro" y lo aclaraba
    // a rayas. Por eso, encontrado el primero, se miran dos anillos mas (4 px
    // de UI, mas que el ancho del suavizado) y el fondo es el mas claro de los
    // tres. Cerca de la raya eso deja la tinta vieja en una franja de 4 px:
    // menos que el grosor del trazo, y no se ve como error.
    float3 bg = col;
    float bgLum = -1.0;
    int firstRing = 0;
    [loop] for (int ring = 1; ring <= 9; ++ring) {
        if (firstRing > 0 && ring > firstRing + 2) break;
        // "Mas claro" es en PROPORCION, no por una cantidad fija: en una zona
        // casi negra (0,04) la letra (0,005) esta a menos de 0,04 del fondo, y
        // con un umbral fijo nunca se la reconocia. A la inversa, un umbral
        // fijo chico convertia en letra cualquier degradado suave del fondo.
        // La letra al 92 % de opacidad es un 8 % del brillo de su fondo; un
        // degradado entre vecinos no llega ni a un 10 % de diferencia.
        float best = max(lum / 0.6 + 0.002, bgLum);
        bool found = false;
        float3 bestCol = bg;
        [unroll] for (int i = 0; i < 8; ++i) {
            float2 o = dirs[i] * (ring * 2.0 * uiScale) * texel;
            // SampleLevel y no Sample: dentro de un bucle que corta segun lo que
            // lee, las derivadas de la UV no existen y Sample elige un mip
            // cualquiera.
            float3 s = AlbedoTex.SampleLevel(AlbedoSampler, uv + o, 0).rgb;
            float l = Luma(s);
            if (l > best) { best = l; bestCol = s; found = true; }
        }
        if (found) {
            bg = bestCol;
            bgLum = best;
            if (firstRing == 0) firstRing = ring;
        }
    }
    if (bgLum < 0.0) return col;                       // no es letra: es fondo
    float dark = smoothstep(darkBelow + 0.1, darkBelow - 0.05, bgLum);
    // Cuanto de letra es este pixel: 0 si es tan claro como el fondo, 1 si es
    // un decimo o menos. Lo de en medio es el borde suavizado.
    float inkAmount = saturate((1.0 - lum / max(bgLum, 1e-4) - 0.1) / 0.8);
    float3 inverted = lerp(bg, float3(lightInk, lightInk, lightInk), inkAmount);
    return lerp(col, inverted, dark);
}

float3 SampleScene(float2 uv, float2 texel, float2 size, float uiScale) {
    float3 col = AlbedoTex.SampleLevel(AlbedoSampler, uv, 0).rgb;
    float2 ui = (uv - 0.5) * size / uiScale;
    [unroll] for (int z = 2; z <= 3; ++z) {
        float4 zone = UserParams[z];
        if (zone.z > zone.x && ui.x >= zone.x && ui.x <= zone.z && ui.y >= zone.y && ui.y <= zone.w)
            return TitleInk(uv, col, texel, uiScale);
    }
    return col;
}

float4 PSMain(PSInput input) : SV_TARGET {
    const float vignette = UserParams[0].x;
    const float radius = UserParams[0].y;
    const float grain = UserParams[0].z;
    const float chroma = UserParams[0].w;
    const float grainFps = max(UserParams[1].x, 1.0);

    float w, h;
    AlbedoTex.GetDimensions(w, h);
    float2 size = float2(w, h);
    float2 texel = 1.0 / size;
    float uiScale = h / 720.0;
    float2 uv = input.uv;
    float2 fromCenter = uv - 0.5;
    // Corregido por aspecto: sin esto la vineta sale ovalada en 16:9.
    float2 aspect = float2(w / h, 1.0);
    float dist = length(fromCenter * aspect) / length(0.5 * aspect);

    // Recortado al borde: el muestreador repite la textura, y sin esto el
    // canal corrido trae una linea del lado opuesto de la imagen.
    float2 shift = fromCenter * dist * chroma * texel * 2.0;
    float2 edge = 0.5 * texel;
    float3 col;
    col.r = SampleScene(clamp(uv + shift, edge, 1.0 - edge), texel, size, uiScale).r;
    col.g = SampleScene(uv, texel, size, uiScale).g;
    col.b = SampleScene(clamp(uv - shift, edge, 1.0 - edge), texel, size, uiScale).b;

    float v = smoothstep(radius, 1.15, dist);
    col *= 1.0 - v * vignette;

    float frame = floor(Time.x * grainFps);
    float n = Hash(floor(uv * size) + frame * 17.0) - 0.5;
    col += n * grain;

    return float4(saturate(col), 1.0);
}
