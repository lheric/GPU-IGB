#pragma OPENCL EXTENSION cl_khr_fp64 : enable

struct CLRay_s{
  float4 color;
  float3 point;
  float3 direction;
  float t_start;
  float t_end;
};
typedef struct CLRay_s CLRay;

struct RaycastParameters_s
{
    float3 center;
    float ambient;
    float diffuse;
    float specular;
    float transparency;
    float f;
    float padding0;
    float padding1;
    float padding2;

};
typedef struct RaycastParameters_s RaycastParameters;


bool intersectBox(float adDim[6], float point[3], float dir[3], float2* tStartEnd)
{
    float tNear = -FLT_MAX;
    float tFar = FLT_MAX;
    const float epslon = 10e-6;

    for(int dim = 0; dim < 3; ++dim)
    {
        float t0, t1;
        if(fabs(dir[dim]) < epslon)
        {
            /// parallel
            if(point[dim] < adDim[2*dim] || point[dim] > adDim[2*dim+1])
            {
                return false;
            }
        }

        t0 = (adDim[2*dim]   - point[dim])/dir[dim];
        t1 = (adDim[2*dim+1] - point[dim])/dir[dim];
        if(t0 > t1)
        {
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        tNear = fmax(tNear, t0);
        tFar  = fmin(tFar, t1);
        if(tNear > tFar)
        {
            return false;
        }
        if(tFar < 0.0)
        {
            return false;
        }
    }
    *tStartEnd = (float2)(fmax(0.0f,tNear), tFar);
    return true;
}

/// original
__kernel void far_to_near(   __read_only image3d_t intensity_vol, 
                            __read_only image3d_t label_vol,
                            __read_only image3d_t label_color_vol,
                            __read_only image3d_t gradient_vol,
                            __global __read_only float4* color_table,
                            RaycastParameters parameters,
                            __write_only image2d_t rendered,
                            __global __read_write int* occlusion_matrix,
                            const int occ_matrix_size 
                            ) 
{ 
    const int width  = get_global_size(0);
    const int height = get_global_size(1);
    
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const float t_step = 0.3;
    const sampler_t linear_sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;
    const sampler_t nearest_sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

    const float rhoAmb = parameters.ambient;   ///0.25
    const float rhoDif = parameters.diffuse;    ///0.4
    const float rhoSpe = parameters.specular;   ///0.77
    const float f = parameters.f;          ///76
    const float transparency = parameters.transparency;

    float dAngleHeight = ((height-1-y)*1.0/height-0.5)*M_PI;
    float dAngleWidth =  (x*1.0/width-0.25)*2*M_PI;

    float3 ray_dir = (float3) ( cos(dAngleWidth)*cos(dAngleHeight),
                            sin(dAngleWidth)*cos(dAngleHeight),
                            sin(dAngleHeight) );
    ray_dir = normalize(ray_dir);
    
    // intersection with box
    float adDim[6]; float point[3]; float dir[3]; float2 tStartEnd;
    adDim[0] = 0; adDim[1] = get_image_width(intensity_vol); 
    adDim[2] = 0; adDim[3] = get_image_height(intensity_vol);
    adDim[4] = 0; adDim[5] = get_image_depth(intensity_vol);
    
    point[0] = parameters.center.x; point[1] = parameters.center.y; point[2] = parameters.center.z;
    
    dir[0] = ray_dir.x; dir[1] = ray_dir.y; dir[2] = ray_dir.z;
    
    bool intersect = intersectBox(adDim, point, dir, &tStartEnd);
    if(!intersect)
    {
      write_imagef(rendered, (int2)(x,y), (float4)(0,0,0,1));
      return;
    }

    float4 intensity; int4 label; float4 color; float3 gradient; float3 half_vector;  
    int last_label = 0;
    float4 sum = 0;
    float t_cur = tStartEnd.x;
    float3 cur_pos;
    float lambert;
    float phong;
    float alpha;
    while(t_cur < tStartEnd.y)
    {
        cur_pos = parameters.center + t_cur*ray_dir;

        //light
        gradient = read_imagef(gradient_vol, linear_sampler, (float4)(cur_pos, 0.0)).xyz;
        gradient = normalize(gradient);
        lambert = fmax(0.0f, dot(gradient, -ray_dir));

        half_vector = -ray_dir; /// light is direction is the ray direction
        phong = fmax(0.0f, dot(gradient, half_vector));

        intensity = read_imagef(intensity_vol, linear_sampler, (float4)(cur_pos, 0.0));
        label =  read_imagei(label_vol, nearest_sampler, (float4)(cur_pos, 0.0));
        //occlusion_matrix[last_label*occ_matrix_size+label.x]++;
        if(label.x != 0)
        {
            atomic_add(occlusion_matrix+last_label*occ_matrix_size+label.x, last_label != label.x);
            last_label = label.x;
        }

        //last_label = label.x; //// BUG select function here, 0 label should be skipped
        color = read_imagef(label_color_vol, linear_sampler, (float4)(cur_pos, 0.0));
        alpha = color.w;
        color = color*rhoAmb + color*rhoDif*lambert + color*rhoSpe*pow(phong, f);
        color = clamp(color, 0.0f, alpha);
        color.w = alpha;
        color *= intensity.x*transparency; 
        sum = color + (1-color.w)*sum;  
        t_cur += t_step;

    }

    write_imagef(rendered, (int2)(x,y), sum);
    //printf("%f\n", sum);
    /*if(x == 10 && y == 11)
    {
        printf("%d %d\n", x, y);
        printf("%f %f\n", parameters.transparency, parameters.specular);
        printf("%f %f %f\n", parameters.center.x, parameters.center.y, parameters.center.z);
        printf("%f %f %f %f\n", sum.x, sum.y, sum.z, sum.w);
        printf("%f %f\n", tStartEnd.x, tStartEnd.y);
        
        printf("%f %f %f %f\n", color_table[0].x, color_table[0].y, color_table[0].z, color_table[0].w);
        printf("%f %f %f %f\n", color_table[1].x, color_table[1].y, color_table[1].z, color_table[1].w);
        printf("%f %f %f %f\n", color_table[5].x, color_table[5].y, color_table[5].z, color_table[5].w);

    }*/
}



__kernel void near_to_far(  __read_only image3d_t intensity_vol, 
                            __read_only image3d_t label_vol,
                            __read_only image3d_t label_color_vol,
                            __read_only image3d_t gradient_vol,
                            __global __read_only float4* color_table,
                            RaycastParameters parameters,
                            __write_only image2d_t rendered,
                            __global __read_write int* occlusion_matrix,
                            const int occ_matrix_size 
                            ) 
{ 
    const int width  = get_global_size(0);
    const int height = get_global_size(1);
    
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const float t_step = 0.3;
    const sampler_t linear_sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;
    const sampler_t nearest_sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

    const float rhoAmb = parameters.ambient;   ///0.25
    const float rhoDif = parameters.diffuse;    ///0.4
    const float rhoSpe = parameters.specular;   ///0.77
    const float f = parameters.f;          ///76
    const float transparency = parameters.transparency;

    float dAngleHeight = ((height-1-y)*1.0/height-0.5)*M_PI;
    float dAngleWidth =  (x*1.0/width-0.25)*2*M_PI;

    float3 ray_dir = (float3) ( cos(dAngleWidth)*cos(dAngleHeight),
                            sin(dAngleWidth)*cos(dAngleHeight),
                            sin(dAngleHeight) );
    ray_dir = normalize(ray_dir);
    
    // intersection with box
    float adDim[6]; float point[3]; float dir[3]; float2 tStartEnd;
    adDim[0] = 0; adDim[1] = get_image_width(intensity_vol); 
    adDim[2] = 0; adDim[3] = get_image_height(intensity_vol);
    adDim[4] = 0; adDim[5] = get_image_depth(intensity_vol);
    
    point[0] = parameters.center.x; point[1] = parameters.center.y; point[2] = parameters.center.z;
    
    dir[0] = ray_dir.x; dir[1] = ray_dir.y; dir[2] = ray_dir.z;
    
    bool intersect = intersectBox(adDim, point, dir, &tStartEnd);
    if(!intersect)
    {
      write_imagef(rendered, (int2)(x,y), (float4)(0,0,0,1));
      return;
    }

    float4 intensity; int4 label; float4 color; float3 gradient; float3 half_vector;  
    int last_label = 0;
    float4 sum = 0;
    float t_cur = tStartEnd.y;
    float3 cur_pos;
    float lambert;
    float phong;
    float alpha;
    while(t_cur > tStartEnd.x)
    {
        cur_pos = parameters.center + t_cur*ray_dir;

        //light
        gradient = read_imagef(gradient_vol, linear_sampler, (float4)(cur_pos, 0.0)).xyz;
        gradient = normalize(gradient);
        lambert = fmax(0.0f, dot(gradient, ray_dir));

        half_vector = ray_dir; /// light is direction is the ray direction
        phong = fmax(0.0f, dot(gradient, half_vector));

        intensity = read_imagef(intensity_vol, linear_sampler, (float4)(cur_pos, 0.0));
        label =  read_imagei(label_vol, nearest_sampler, (float4)(cur_pos, 0.0));
        //occlusion_matrix[last_label*occ_matrix_size+label.x]++;
        if(label.x != 0)
        {
            atomic_add(occlusion_matrix+last_label*occ_matrix_size+label.x, last_label != label.x);
            last_label = label.x;
        }

        //last_label = label.x; //// BUG select function here, 0 label should be skipped
        color = read_imagef(label_color_vol, linear_sampler, (float4)(cur_pos, 0.0));
        alpha = color.w;
        color = color*rhoAmb + color*rhoDif*lambert + color*rhoSpe*pow(phong, f);
        color = clamp(color, 0.0f, alpha);
        color.w = alpha;
        color *= intensity.x*transparency; 
        sum = color + (1-color.w)*sum;  
        t_cur -= t_step;

    }

    write_imagef(rendered, (int2)(x,y), sum);
    //printf("%f\n", sum);
    /*if(x == 10 && y == 11)
    {
        printf("%d %d\n", x, y);
        printf("%f %f\n", parameters.transparency, parameters.specular);
        printf("%f %f %f\n", parameters.center.x, parameters.center.y, parameters.center.z);
        printf("%f %f %f %f\n", sum.x, sum.y, sum.z, sum.w);
        printf("%f %f\n", tStartEnd.x, tStartEnd.y);
        
        printf("%f %f %f %f\n", color_table[0].x, color_table[0].y, color_table[0].z, color_table[0].w);
        printf("%f %f %f %f\n", color_table[1].x, color_table[1].y, color_table[1].z, color_table[1].w);
        printf("%f %f %f %f\n", color_table[5].x, color_table[5].y, color_table[5].z, color_table[5].w);

    }*/
}