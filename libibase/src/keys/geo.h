#ifndef NANOS_GEOENCODE_H
#define NANOS_GEOENCODE_H

#include <cstring>
#include <cassert>
#include <string>
#include <cmath>
#include <stdint.h>

using namespace std;
namespace geoencode{

#define  LEVEL  6

class CDivConst  
{
public:
	CDivConst();
public:
  	int m_nRatio;   //����3600000�� ƽ��ÿ��3600000/111120(GDAL) = 0.03��
	int m_nLonDiv;
	int m_nLatDiv;
};

class CGridCode
{
public:
	CGridCode();
	~CGridCode();

private:
    static int m_nGridWidths[91];
	static int m_nDivLayer[LEVEL];
	static int m_nDivDist[LEVEL];
    int m_nGridLatLon[LEVEL][2];//0:γ��,1:����

    CDivConst  m_objConst;
    const long m_nShift;//ƫ��
    int m_nLonEW; //��������
    int m_nLatNS; //�ϱ�γ

public:
	bool set(const long& lg);
    long get(double lon, double lat);
    /*���ƾ���*/
	int appro(const CGridCode& obj);
    /*��ȷ����*/
    static int exact(double lon1, double lon2, double lat1, double lat2);
private:
	long combine();
};

/*
 *Copyright (c) 2013-2014, yinqiwen <yinqiwen@gmail.com>
 *All rights reserved.
 */

#if defined(__cplusplus)
extern "C"
{
#endif

typedef struct
{
   uint64_t bits;
   uint8_t step;
}GeoHashBits;

typedef struct
{
   double max;
   double min;
}GeoHashRange;

class GeoHash{
public:
	GeoHash(uint8_t step = 30);
    /*
     * 0:success
     * -1:failed
     */
    int encode(double lonitude, double latitude, uint64_t& hash) const;
    int decode(const uint64_t& hash, GeoHashRange& longtude, GeoHashRange& latitude);

private:
    uint8_t get_bit(uint64_t bits, uint8_t pos);

private:
  const uint8_t m_step;
  GeoHashRange m_latRange, m_lonRange;
};
#if defined(__cplusplus)
}
#endif

}
#endif
