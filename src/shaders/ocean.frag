#version 450 core

in vec4 colorFragIn;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;

out vec4 color;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.f, 1.0f, -1.0f));
    vec3 lightColor = vec3(0.69f, 0.84f, 1.0f);
    float diffuse = max(dot(norm, lightDir), 0.0);

    float reflectivity;
    float nSnell = 1.34f;
    float costhetai = abs(dot(lightDir, norm));
    float thetai = acos(costhetai);
    float sinthetat = sin(thetai)/nSnell;
    float thetat = asin(sinthetat);
    if(thetai == 0.0)
    {
        reflectivity = (nSnell - 1)/(nSnell + 1);
        reflectivity = reflectivity * reflectivity;
    }
    else
    {
        float fs = sin(thetat - thetai) / sin(thetat + thetai);
        float ts = tan(thetat - thetai) / tan(thetat + thetai);
        reflectivity = 0.5 * ( fs*fs + ts*ts );
    }


    float ambient = 0.1f;

    float specular = 0.0f;
    if(diffuse > 0.01f)
    {
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        specular = pow(max(dot(viewDir, reflectDir), 0.0), 32) * 0.01f;
    }

    color = colorFragIn * vec4((ambient + reflectivity + specular) * lightColor, 1.0f);
}