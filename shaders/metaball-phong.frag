// https://github.com/orglofch/metaballs/blob/master/Metaballs/metaball_shader_3d.frag

varying vec4 fColor;

#define MAX_CHARGES 30

#define EPSILON 0.01
#define INF 1.0 / 0.0
#define MAX_REFLECTIONS 2

#define INTERSECTION_ENTRY
#define INTERSECTION_ENTRANCE

// May not actually be full - contains positions of charges and radii of them
uniform vec4 charges[MAX_CHARGES];

uniform int numCharges;

uniform mat4 matView;
uniform vec3 cameraPos;
uniform vec3 lightDir;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;

/**
 * Calculates the intersections with the given charge sphere.
 *
 * @param ro The origin of the cast ray.
 * @param dir The direction of the cast ray.
 * @param index The index of the charge sphere to intersect.
 * @param intersections The structures for storing the intersection results.
 * @return The number of intersections found.
 */
int findSphereIntersection(
    in vec3 ro,
    in vec3 dir,
    in int index,
    out float roots[2]
) {
    vec3 fc = ro - charges[index].xyz;

    float a = dot(dir, dir);
    float b = 2.0 * dot(fc, dir);
    float c = dot(fc, fc) - charges[index].w * charges[index].w;

    if (a == 0)
    {
        if (b == 0)
        {
            return 0;
        }
        else
        {
            roots[0] = -c / b;
            return 1;
        }
    }
    else
    {
        float det = b * b - 4.0 * a * c;
        if (det <= 0.0)
        {
            return 0;
        }
        else
        {
            float q = -(b + sign(b) * sqrt(det)) / 2.0;
            roots[0] = q / a;
            if (q != 0)
            {
                roots[1] = c / q;
            }
            else
            {
                roots[1] = roots[0];
            }
            return 2;
        }
    }
    return 0;
}

float calculateCharge(in float distance, in float charge_radius)
{
    float r = distance / charge_radius;
    if (r > 1)
    {
        return 0;
    }

    return 1.0 - r * r * r * (r * ( r * 6.0 - 15.0) + 10.0);
}

bool findIntersection(in vec3 ro, in vec3 rd, out vec3 pos, out vec3 normal)
{
    // Find intersection with the ray and the metaball spheres.
    // The charge function we use = 0 for dist > charge radius
    //  so we can ignore charges which fall outside this range
    int active_charges[MAX_CHARGES];
    int intersection_count = 0;

    // Keep track of the min and max t values so we only have to
    //  iterate between the two points these define.
    // There may be work to be done here! See https://github.com/orglofch/metaballs/blob/76a139156de4fc76175153ee7e67d1526cfd40c5/Metaballs/metaball_shader_3d.frag#L90
    float min_t = INF;
    float max_t = 0.0;

    for (int i = 0; i < charge_count; ++i)
    {
        float roots[2];
        int num_roots = findSphereIntersection(ro, rd, i, roots);
        if (num_roots > 0)
        {
            active_charges[intersection_count++] = i;
            if (num_roots >= 1 && roots[0] > 0)
            {
                max_t = max(max_t, roots[0]);
                min_t = min(min_t, roots[0]);
            }
            if (num_roots == 2 && roots[1] > 0)
            {
                max_t = max(max_t, roots[1]);
                min_t = min(min_t, roots[1]);
            }
        }
    }

    if (intersection_count == 0)
    {
        return false;
    }

    // Ray march between [min_t, max_t] to approximate if this
    //  ray ever intersects a meta surface
    float cur_t = min_t;
    while (cur_t < max_t)
    {
        pos = ro + rd * cur_t;
        float step_charge = 0.0;
        for (int i = 0; i < intersection_count; ++i)
        {
            vec4 metaball = charges[active_charges[i]];
            float dist = distance(metaball.xyz, pos);
            step_charge += calculateCharge(dist, metaball.w);
        }

        if (step_charge >= 0.4)
        {
            normal = vec3(0, 0, 0);
            // Add normals for each metaball based on their contribution
            for (int i = 0; i < intersection_count; ++i)
            {
                vec4 metaball = charges[active_charges[i]];
                vec3 from_center = pos - metaball.xyz;
                float dist = length(from_center);
                float charge = calculateCharge(dist, metaball.w);
                if (charge > 0.0)
                {
                    normal += normalize(from_center) * (charge / step_charge);
                }
            }
            normal = normalize(normal);
            return true;
        }
        // Take larger steps the lower our step charge
        //  (the farther we are from the meta surface)
        float step = step_charge;
        cur_t += 0.5; // TODO SESS: You could use a fancier formula here...
    }

    return false;
}

vec4 colorForIntersection(in vec3 rd, in vec3 pos, in vec3 normal)
{
    // TODO SESS: Fill this in
    return vec4(1.0, 0.0, 0.0, 1.0);
}

void main()
{
    // TODO SESS: Use RH coordinates or LH? Original uses LH
    vec3 pixel_in_world = (matView * vec4(gl_FragCoord.xy, 0, 1)).xyz;

    vec3 ro = cameraPos;
    vec3 rd = normalize(pixel_in_world - cameraPos);

    vec4 final_color = vec4(0.0);
    float color_frac = 1.0;
    for (int i = 0; i < MAX_REFLECTIONS + 1; ++i)
    {
        vec3 pos, normal;
        if (!findIntersection(ro, rd, pos, normal))
        {
            break;
        }

        vec4 color = colorForIntersection(rd, pos, normal);

        // Set parameters for next iteration
        color_frac = color_frac * 0.2;
        ro = pos + normal;
        rd = normalize(reflect(rd, normal));
    }

    // TODO SESS: Adjust this for blending
    gl_FragCoord = vec4(final_color.xyz, 1.0);
}