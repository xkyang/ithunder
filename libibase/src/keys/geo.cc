#include "geo.h"

#define abs(a) ((a) < 0 ? (-a) : (a))

namespace geoencode{
int	CGridCode::m_nGridWidths[] = {
      463, 462, 462, 462, 461, 461, 460, 459, 458, 457, 455, 454, 452, 451, 449,
      447, 445, 442, 440, 437, 435, 432, 429, 426, 422, 419, 416, 412, 408, 404,
      400, 396, 392, 388, 383, 379, 374, 369, 364, 359, 354, 349, 344, 338, 333,
      327, 321, 315, 309, 303, 297, 291, 285, 278, 272, 265, 258, 252, 245, 238,
      231, 224, 217, 210, 202, 195, 188, 180, 173, 165, 158, 150, 143, 135, 127,
      119, 112, 104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16, 8, 0};
int CGridCode::m_nDivLayer[] = {8, 10, 3, 5, 10, 1};
int CGridCode::m_nDivDist[] = {240, 30, 3, 1, 0, 0};

CGridCode::CGridCode():m_nShift(100000000000000000l),m_nLonEW(1),m_nLatNS(1)
{
   memset(m_nGridLatLon,0,LEVEL * 2 * sizeof(int));	
}

CGridCode::~CGridCode()
{
}

bool CGridCode::set(const long& lg)
{
	long nShift = m_nShift;
	long m_nLatNS = lg / nShift; 
	if((m_nLatNS < 1) || (m_nLatNS > 2))
	{
	   return false;
	}
	long tempLg = lg % nShift;
	nShift /= 10;
	long m_nLonWE = tempLg / nShift; 
	if((m_nLonWE < 1) || (m_nLonWE > 2))
	{
	   return false;
	}
	tempLg %= nShift;
	nShift /= 1000;
	m_nGridLatLon[0][0] = tempLg / nShift;
	if((m_nGridLatLon[0][0] < 1) || (m_nGridLatLon[0][0] > 135))
	{
	   return false;
	}
	tempLg %= nShift;
	nShift /= 1000;
	m_nGridLatLon[0][1] = tempLg / nShift;
	if((m_nGridLatLon[0][1] < 1) || (m_nGridLatLon[0][1] > 180))
	{
	   return false;
	}
	for(int i=1;i<LEVEL;++i)
	{
	   tempLg %= nShift;
	   nShift /= 10;
	   m_nGridLatLon[i][0] = tempLg / nShift;
	   if((m_nGridLatLon[i][0] < 0) || (m_nGridLatLon[i][0] >= m_nDivLayer[i - 1]))
	   {
	     return false;
	   }
	   tempLg %= nShift;
	   nShift /= 10;
	   m_nGridLatLon[i][1] = tempLg / nShift;
	   if((m_nGridLatLon[i][1] < 0) || (m_nGridLatLon[i][1] >= m_nDivLayer[i - 1]))
	   {
	     return false;
	   }
	}
    return true;
}

 /* Lon:经度,
  *   本初子午线的经度是0°,地球上其它地点的经度是向东到180°或向西到180°;
  *   东经(E)为正数，西经(W)为负数;
  *   取值范围[-180,180];
  *   每度大约相当于111km(111120);
  * Lat:纬度,其数值在0至90度之间; 
  *   位于赤道以北的点的纬度叫北纬，记为N;
  *   位于赤道以南的点的纬度称南纬，记为S;
  *   取值范围[0,90];
  *   每度的距离从0km到111km不等
  *   地球的子午线总长度大约40008km.平均:
  *   纬度1度 = 大约111km
  *   纬度1分 = 大约1.85km
  *   纬度1秒 = 大约30.9m
  * * 
  *   china_height_ = 4200; // 中国南北长4200km
  *   china_width_  = 5200; // 中国东西长5200km
  *   min_lat_ = 16.5;      // 中国最小纬度
  *   max_lat_ = 53.6;      // 中国最大纬度
  *   min_lng_ = 73.0;      // 中国最小经度
  *   max_lng_ = 135.0;     // 中国最大经度
  * */
long CGridCode::get(double x, double y)
{
	int nPosLon = static_cast<int>(x * m_objConst.m_nRatio/*3600000*/);
	int nPosLat = static_cast<int>(y * m_objConst.m_nRatio/*3600000*/);
    if(nPosLon < 0)
	{
       m_nLonEW = 2; //South
       nPosLon = -nPosLon;
    }
    else
	{
       m_nLonEW = 1; //North
    }
    if(nPosLat < 0)
	{
       m_nLatNS = 2; //West
       nPosLat = -nPosLat;
    }
    else
	{
       m_nLatNS = 1; //East
    }
    //cout << "(" << x << "," << y << ") => (" << nPosLon << "," << nPosLat << ")" << endl;
	int nDivLon = m_objConst.m_nLonDiv/*3600000*/;
	int nDivLat = m_objConst.m_nLatDiv/*2400000*/;
	
    //开始划分
	/*
	 * 第一级
     *  Lon:取值范围[-180,180],绝对值后,Lon[0]取值范围[0,180];
     *  Lat:取值范围[0,90],放大1.5倍(36/24)后,Lat[0]取值范围[0,135];
	 * 第二级
	 *  Lon:取值范围[0,3600000},取余450000,Lon[1]的取值范围[0,8}
	 *  Lat:取值范围[0,2400000},取余300000,Lat[1]的取值范围[0,8}
     * 第三级
	 *  Lon:取值范围[0,450000},取余45000,Lon[2]的取值范围[0,10}
	 *  Lat:取值范围[0,300000},取余30000,Lat[2]的取值范围[0,10}
     * 第四级
	 *  Lon:取值范围[0,45000},取余15000,Lon[3]的取值范围[0,3}
	 *  Lat:取值范围[0,30000},取余10000,Lat[3]的取值范围[0,3}
     * 第五级
	 *  Lon:取值范围[0,15000},取余1500,Lon[3]的取值范围[0,10}
	 *  Lat:取值范围[0,10000},取余1000,Lat[3]的取值范围[0,10}
	 */
	int i = 0;
	do
    {
        m_nGridLatLon[i][0] = nPosLat / nDivLat;
        m_nGridLatLon[i][1] = nPosLon / nDivLon;
        nPosLat %= nDivLat;
        nPosLon %= nDivLon;
	    nDivLat /= m_nDivLayer[i];
	    nDivLon /= m_nDivLayer[i];
		//cout << m_nGridLatLon[i][0] << "\t" << m_nGridLatLon[i][1] << "\t" << nPosLon << "\t" << nPosLat << "\t" << nDivLon << "\t" << nDivLat << endl; 
		++i;
	}while(i < LEVEL);

	return combine();
}

long CGridCode::combine()
{
	int bits = 10;
    long nShift = m_nShift;
	long lg = m_nLatNS * nShift;
	nShift /= bits;
	lg += m_nLonEW * nShift;
	for(int i=0;i<LEVEL;++i)
	{
	  if(0 == i)
	  {
	     bits = 1000;
	  }
	  else
	  {
	     bits = 10;
	  }
	  nShift /= bits;
	  lg += m_nGridLatLon[i][0] * nShift;
	  nShift /= bits;
	  lg += m_nGridLatLon[i][1] * nShift;
	}
    return lg;
}

//取得两点间的精确距离
int CGridCode::exact(double lon1, double lon2, double lat1, double lat2) 
{
	double dc = 1.852;
    double de = 0.0174533;
    double dd = 57.29578;
	lon1 = de * lon1;
    lon2 = de * lon2;
    lat1 = de * lat1;
    lat2 = de * lat2;
    if((lat1 + lat2 == 0.) && (abs(lon1 - lon2) == 3.1415926))
	{
      return 0;
    }
	/*
    double t1 = sin(lat1) * sin(lat2) ;
    double t2 = cos(lat1) * cos(lat2) * cos(lon1 - lon2);
	*/
    double d = acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon1 - lon2)) * 1000 * dd * 60 * dc;
	return (int)d;
}

int CGridCode::appro(const CGridCode& obj)
{
	int heightWidth[2] = {0, 0};
	for(int i=0;i<LEVEL;++i)
	{
       heightWidth[0] += (obj.m_nGridLatLon[i][0] - m_nGridLatLon[i][0]) * m_nDivDist[i];//lat
       heightWidth[1] += (obj.m_nGridLatLon[i][1] - m_nGridLatLon[i][1]) * m_nDivDist[i];//lon
	}
    int yWidth = m_nGridWidths[obj.m_nGridLatLon[0][0] * 2 / 3];
    double w = heightWidth[1] * yWidth;
    double h = heightWidth[0] * 308;
    return static_cast<int>(sqrt(w * w + h * h)); 
}

CDivConst::CDivConst()
{
   m_nRatio = 3600000;   //精度3600000 平均每点111120/m_nLonDiv= 0.03米
   m_nLonDiv = 3600000;
   m_nLatDiv = 2400000;
}

/*
 *Copyright (c) 2013-2014, yinqiwen <yinqiwen@gmail.com>
 *All rights reserved.
 *
 * Hashing works like this:
 * Divide the world into 4 buckets.  Label each one as such:
 *  -----------------
 *  |       |       |
 *  |       |       |
 *  | 0,1   | 1,1   |
 *  -----------------
 *  |       |       |
 *  |       |       |
 *  | 0,0   | 1,0   |
 *  -----------------
 */

GeoHash::GeoHash(uint8_t step):m_step(step)
{
    m_latRange.max = 90;
    m_latRange.min = -90;
    m_lonRange.max = 180;
    m_lonRange.min = -180;
}
	
int GeoHash::encode(double longitude, double latitude, uint64_t& hash) const
{
    if (m_step > 32 || m_step == 0)
    {
        return -1;
    }
    if (latitude < m_latRange.min || latitude > m_latRange.max
     || longitude < m_lonRange.min || longitude > m_lonRange.max)
    {
        return -1;
    }
    hash = 0;
	GeoHashRange latRange = m_latRange;
	GeoHashRange lonRange = m_lonRange;
    uint8_t lat_bit = 0;
	uint8_t lon_bit = 0;
    for (uint8_t i = 0; i < m_step; i++)
    {
        if (latRange.max - latitude >= latitude - latRange.min)
        {
            lat_bit = 0;
            latRange.max = (latRange.max + latRange.min) / 2;
        }
        else
        {
            lat_bit = 1;
            latRange.min = (latRange.max + latRange.min) / 2;
        }
        if (lonRange.max - longitude >= longitude - lonRange.min)
        {
            lon_bit = 0;
            lonRange.max = (lonRange.max + lonRange.min) / 2;
        }
        else
        {
            lon_bit = 1;
            lonRange.min = (lonRange.max + lonRange.min) / 2;
        }
        hash <<= 1;
        hash += lon_bit;
        hash <<= 1;
        hash += lat_bit;
    }
    return 0;
}

uint8_t GeoHash::get_bit(uint64_t bits, uint8_t pos)
{
    return (bits >> pos) & 0x01;
}

int GeoHash::decode(const uint64_t& hash, GeoHashRange& longitude, GeoHashRange& latitude)
{
    longitude.min = m_lonRange.min;
    longitude.max = m_lonRange.max;
    latitude.min = m_latRange.min;
    latitude.max = m_latRange.max;
    for (uint8_t i = 0; i < m_step; i++)
    {
		uint8_t step = m_step - i;
        uint8_t lon_bit = get_bit(hash, step * 2 - 1);
        uint8_t lat_bit = get_bit(hash, step * 2 - 2);
        if (lat_bit == 0)
        {
            latitude.max = (latitude.max + latitude.min) / 2;
        }
        else
        {
            latitude.min = (latitude.max + latitude.min) / 2;
        }
        if (lon_bit == 0)
        {
            longitude.max = (longitude.max + longitude.min) / 2;
        }
        else
        {
            longitude.min = (longitude.max + longitude.min) / 2;
        }
    }
    return 0;
}

}
