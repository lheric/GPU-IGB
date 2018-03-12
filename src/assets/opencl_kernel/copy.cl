typedef struct CLRay_s{
  float3 point;
  float3 direction;
  float4 color;
  float t_start;
  float t_end;
  bool hit;
}CLRay;

__kernel void image_copy(__read_only image3d_t intensity_vol, __global CLRay *rays) 
{ 
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int z = get_image_depth(intensity_vol);
    const float t_step = 1.0;
    const sampler_t sampler=CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;
    
    int offset = y * get_global_size(0) + x;
    __global CLRay* ray = rays + offset;


    float4 pixel;
    
    float4 sum = 0;
    /*for(int i = 0; i < z; i++)
    {
      pixel = read_imagef(intensity_vol, sampler, (float4)(x,y,i,0));
      sum += pixel;
    }*/

    float t_cur = ray->t_end;
    float3 cur_pos;
    while(t_cur > ray->t_start)
    {
      cur_pos = ray->point + t_cur*ray->direction;
      pixel = read_imagef(intensity_vol, sampler, (float4)(cur_pos, 0.0));
      sum += pixel;
      t_cur -= t_step;
    }

    sum /= z;
    ray->color = sum;
    //write_imagef(image2, (int2)(x,y), sum);
    //printf("%f\n", sum);

  //printf("%d %d %f\n", x, y, sum.x);
}