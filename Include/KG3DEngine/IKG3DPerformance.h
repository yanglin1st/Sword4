/*!
 * \file IKG3DPerformance.h
 * \date 2014/07/14 10:05
 * \author Gu Lu (gulu@kingsoft.com)
 *
 * \brief Definition of IKG3DPerformance 
*/

#pragma once

#include <tchar.h>


/* ----- ������صĲ��� ----- */
// �ж��Ƿ��������У�O(n)��
template <typename ELEM, typename CONT> bool KPerfIsInContainer(ELEM e, CONT c);


/* ----- ��ɫ��صĲ��� ----- */
/*
    ���ٶ����ʹ�ñ�׼��ɫ 

    ��������Զ��ѱ���������ɫֵ(Color Value)����ɫ��(Color Name)�ҹ���
    �����ʹ����ɫ�Ĵ���Ŀɶ��ԡ�

    �÷���
    KG_COLOR(White)         ���ذ�ɫ��ֵ:     0xffffffff
    KG_COLOR_NAME(White)    ���ذ�ɫ����ɫ��:  _T("White")
*/

// �������ڱ�ͷ�ļ��ڼ��ж���
#define KG_DEFINE_COLOR(c, v) \
    const int               KG_COLOR_##c = v; \
    const TCHAR* const      KG_COLOR_NAME_##c = _T(#c);
// ȡ��ɫֵ
#define KG_COLOR(c)         KG_COLOR_##c  
// ȡ��ɫ���ַ���
#define KG_COLOR_NAME(c)    KG_COLOR_NAME_##c  

/* ----- ������ɫ���� ----- */
KG_DEFINE_COLOR(Black,          0xff000000);
KG_DEFINE_COLOR(White,          0xffffffff);
KG_DEFINE_COLOR(Red,            0xffff0000);
KG_DEFINE_COLOR(Green,          0xff00ff00);
KG_DEFINE_COLOR(Blue,           0xff0000ff);
KG_DEFINE_COLOR(Yellow,         0xffffff00);
KG_DEFINE_COLOR(BrightYellow,   0xffffffc0);
KG_DEFINE_COLOR(Purple,         0xffff00ff);
KG_DEFINE_COLOR(Indigo,         0xff00ffff);
KG_DEFINE_COLOR(Gray,           0xff808080);
KG_DEFINE_COLOR(TransWhite,     0x80ffffff);
KG_DEFINE_COLOR(TransGray,      0x80404040);
KG_DEFINE_COLOR(MoreTransGray,  0x40202020);
KG_DEFINE_COLOR(LessTransGray,  0xc0202020);


/* ----- Math - ������ѧ�� ----- */
const float KPERF_Epsilon = 0.00001f;
template <typename T> T KPerfEqual(T v1, T v2);
template <typename T> T KPerfClamp(T val, T minV, T maxV);
template <typename T> T KPerfLerp(T val, T minV, T maxV);


/* ----- ���ܵȼ����� ----- */
enum ePerfLevel
{
    PLV_None,
    PLV_Smooth,        // average ~ 60
    PLV_Tolerable,     // 30-60
    PLV_Critical,      // less than 30
    PLV_Num,        
};

const char* const g_perfLevelInfo[] = 
{
    "None",
    "Smooth",
    "Tolerable",
    "Critical",
};

/* ----- �����Ż�ѡ��ɵ������أ� ----- */
enum ePerfOptFeatrues
{
    POF_None            = 0,
    POF_FilterPlayers   = 1,
    POF_FilterEntities  = 1 << 1,
    POF_PostCull        = 1 << 2,
    POF_All             = 0xffffffff,
};

enum ePerfStatsType
{
    PST_TotalCount,
    PST_CulledCount,
    PST_AffectedCount,
    PST_Num,
};

/*
 *	�ӿ��� - �����¼�������� ����������ϵͳ����ͨ���̳�������࣬��Ӧ�����¼���
 */
interface IPerfEventHandler
{
    // ��Ҫ�����¼�
    virtual void OnPerfLevelChanged(ePerfLevel newLevel, ePerfLevel oldLevel) = 0;
    virtual void OnPerfLevelWarning(ePerfLevel newLevel, double fTime) = 0;

    // ��Ҫ�ӿڣ���ѡ��
    virtual void Refresh(double fTime, const D3DXVECTOR3& vPlayerPos) {}
    virtual std::map<ePerfStatsType, DWORD> GetStats() { return std::map<ePerfStatsType, DWORD>(); }   
};

interface IPerfEventHandlerSet
{
    virtual void Register(IPerfEventHandler* peh) = 0;
    virtual IPerfEventHandler* GetPlayerHandler() = 0;
};

interface IKG3DPerformanceTweaker
{
    virtual unsigned int GetEngineFPS() = 0;
    virtual void RenderDebugInfo() = 0;

    virtual void SetEnable(bool bEnable) = 0;
    virtual bool IsEnabled() = 0;

    virtual void ExecuteCommand(const char* szCommnd) = 0;
    virtual void RegisterHandlerSet(IPerfEventHandlerSet* handerSet) = 0;

    virtual bool IsOptFeatureEnabled(ePerfOptFeatrues pof) = 0;
	virtual void PrintLaptopTag() = 0;

    virtual void SetAllPlayersVisible(bool visible) = 0;
};


//-----------------------------------------------------------------------------
/* ----- ����Ϊʵ��ϸ�� ----- */

template <typename ELEM, typename CONT>
bool KPerfIsInContainer(ELEM e, CONT c) 
{
    return std::find(c.begin(), c.end(), e) != c.end(); 
}

template <typename T> T KPerfEqual(T v1, T v2)
{ 
    return abs(v1 - v2) < (T)(KPERF_Epsilon); 
}
template <typename T> T KPerfClamp(T val, T minV, T maxV) 
{ 
    if (minV > maxV)
    {
        return KPerfClamp(val, maxV, minV);
    }
    else
    {
        return val < minV ? minV : (val > maxV ? maxV : val); 
    }
}
template <typename T> T KPerfLerp(T val, T minV, T maxV)
{
    if (minV > maxV)
    {
        return KPerfLerp(val, maxV, minV);
    } 
    else if (KPerfEqual(maxV, minV))
    {
        return (val >= minV) ? 1.0f : 0.0f;
    } 
    else
    {
        return (KPerfClamp(val, minV, maxV) - minV) / (maxV - minV); 
    }
}
