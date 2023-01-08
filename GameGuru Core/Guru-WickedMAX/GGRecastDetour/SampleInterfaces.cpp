#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "SampleInterfaces.h"
#include "Recast.h"
#include "RecastDebugDraw.h"
#include "DetourDebugDraw.h"
#include "PerfTimer.h"
#include "SDL.h"
#include "SDL_opengl.h"

// For debug object creation calls
#include "M-Waypoint.h"

#ifdef WIN32
#	define snprintf _snprintf
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

BuildContext::BuildContext() :
	m_messageCount(0),
	m_textPoolSize(0)
{
	memset(m_messages, 0, sizeof(char*) * MAX_MESSAGES);

	resetTimers();
}

// Virtual functions for custom implementations.
void BuildContext::doResetLog()
{
	m_messageCount = 0;
	m_textPoolSize = 0;
}

void BuildContext::doLog(const rcLogCategory category, const char* msg, const int len)
{
	if (!len) return;
	if (m_messageCount >= MAX_MESSAGES)
		return;
	char* dst = &m_textPool[m_textPoolSize];
	int n = TEXT_POOL_SIZE - m_textPoolSize;
	if (n < 2)
		return;
	char* cat = dst;
	char* text = dst+1;
	const int maxtext = n-1;
	// Store category
	*cat = (char)category;
	// Store message
	const int count = rcMin(len+1, maxtext);
	memcpy(text, msg, count);
	text[count-1] = '\0';
	m_textPoolSize += 1 + count;
	m_messages[m_messageCount++] = dst;
}

void BuildContext::doResetTimers()
{
	for (int i = 0; i < RC_MAX_TIMERS; ++i)
		m_accTime[i] = -1;
}

void BuildContext::doStartTimer(const rcTimerLabel label)
{
	m_startTime[label] = getPerfTime();
}

void BuildContext::doStopTimer(const rcTimerLabel label)
{
	const TimeVal endTime = getPerfTime();
	const TimeVal deltaTime = endTime - m_startTime[label];
	if (m_accTime[label] == -1)
		m_accTime[label] = deltaTime;
	else
		m_accTime[label] += deltaTime;
}

int BuildContext::doGetAccumulatedTime(const rcTimerLabel label) const
{
	return getPerfTimeUsec(m_accTime[label]);
}

void BuildContext::dumpLog(const char* format, ...)
{
	// Print header.
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
	printf("\n");
	
	// Print messages
	const int TAB_STOPS[4] = { 28, 36, 44, 52 };
	for (int i = 0; i < m_messageCount; ++i)
	{
		const char* msg = m_messages[i]+1;
		int n = 0;
		while (*msg)
		{
			if (*msg == '\t')
			{
				int count = 1;
				for (int j = 0; j < 4; ++j)
				{
					if (n < TAB_STOPS[j])
					{
						count = TAB_STOPS[j] - n;
						break;
					}
				}
				while (--count)
				{
					putchar(' ');
					n++;
				}
			}
			else
			{
				putchar(*msg);
				n++;
			}
			msg++;
		}
		putchar('\n');
	}
}

int BuildContext::getLogCount() const
{
	return m_messageCount;
}

const char* BuildContext::getLogText(const int i) const
{
	return m_messages[i]+1;
}

/*
////////////////////////////////////////////////////////////////////////////////////////////////////

class GLCheckerTexture
{
	unsigned int m_texId;
public:
	GLCheckerTexture() : m_texId(0)
	{
	}
	
	~GLCheckerTexture()
	{
		if (m_texId != 0)
			glDeleteTextures(1, &m_texId);
	}
	void bind()
	{
		if (m_texId == 0)
		{
			// Create checker pattern.
			const unsigned int col0 = duRGBA(215,215,215,255);
			const unsigned int col1 = duRGBA(255,255,255,255);
			static const int TSIZE = 64;
			unsigned int data[TSIZE*TSIZE];
			
			glGenTextures(1, &m_texId);
			glBindTexture(GL_TEXTURE_2D, m_texId);

			int level = 0;
			int size = TSIZE;
			while (size > 0)
			{
				for (int y = 0; y < size; ++y)
					for (int x = 0; x < size; ++x)
						data[x+y*size] = (x==0 || y==0) ? col0 : col1;
				glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, size,size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				size /= 2;
				level++;
			}
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, m_texId);
		}
	}
};
GLCheckerTexture g_tex;
*/

void DebugDrawMAX::depthMask(bool state)
{
	//glDepthMask(state ? GL_TRUE : GL_FALSE);
}

void DebugDrawMAX::texture(bool state)
{
	//if (state)
	//{
	//	glEnable(GL_TEXTURE_2D);
	//	g_tex.bind();
	//}
	//else
	//{
	//	glDisable(GL_TEXTURE_2D);
	//}
}

void DebugDrawMAX::begin(duDebugDrawPrimitives prim, float size)
{
	switch (prim)
	{
		case DU_DRAW_POINTS:
			//glPointSize(size);
			//glBegin(GL_POINTS);
			break;
		case DU_DRAW_LINES:
			//glLineWidth(size);
			//glBegin(GL_LINES);
			m_iDebugObjectType = DU_DRAW_LINES;
			m_DebugObjectVertList.clear();
			break;
		case DU_DRAW_TRIS:
			//glBegin(GL_TRIANGLES);
			m_iDebugObjectType = DU_DRAW_TRIS;
			m_DebugObjectVertList.clear();
			break;
		case DU_DRAW_QUADS:
			//glBegin(GL_QUADS);
			break;
	};
}

void DebugDrawMAX::vertex(const float* pos, unsigned int color)
{
	//glColor4ubv((GLubyte*)&color);
	//glVertex3fv(pos);
	sDebugObjectVert DebugObjectVert;
	DebugObjectVert.x = *(pos + 0);
	DebugObjectVert.y = *(pos + 1);
	DebugObjectVert.z = *(pos + 2);
	DebugObjectVert.color = color;
	//if (m_DebugObjectVertList.size() < 10000 * 3)//21840*3)
	//{
		m_DebugObjectVertList.push_back(DebugObjectVert);
	//}
}

void DebugDrawMAX::vertex_raised(const float* pos, unsigned int color, float fRaiseY)
{
	sDebugObjectVert DebugObjectVert;
	DebugObjectVert.x = *(pos + 0);
	DebugObjectVert.y = (*(pos + 1)) + fRaiseY;
	DebugObjectVert.z = *(pos + 2);
	DebugObjectVert.color = color;
	m_DebugObjectVertList.push_back(DebugObjectVert);
}

void DebugDrawMAX::vertex(const float x, const float y, const float z, unsigned int color)
{
	//glColor4ubv((GLubyte*)&color);
	//glVertex3f(x,y,z);
	sDebugObjectVert DebugObjectVert;
	DebugObjectVert.x = x;
	DebugObjectVert.y = y;
	DebugObjectVert.z = z;
	DebugObjectVert.color = color;
	//if (m_DebugObjectVertList.size() < 10000 * 3)//21840*3)
	//{
		m_DebugObjectVertList.push_back(DebugObjectVert);
	//}
}

void DebugDrawMAX::vertex(const float* pos, unsigned int color, const float* uv)
{
	//glColor4ubv((GLubyte*)&color);
	//glTexCoord2fv(uv);
	//glVertex3fv(pos);
	sDebugObjectVert DebugObjectVert;
	DebugObjectVert.x = *(pos + 0);
	DebugObjectVert.y = *(pos + 1);
	DebugObjectVert.z = *(pos + 2);
	DebugObjectVert.color = color;
	//if (m_DebugObjectVertList.size() < 10000 * 3)//21840*3)
	//{
		m_DebugObjectVertList.push_back(DebugObjectVert);
	//}
}

void DebugDrawMAX::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
	//glColor4ubv((GLubyte*)&color);
	//glTexCoord2f(u,v);
	//glVertex3f(x,y,z);
	sDebugObjectVert DebugObjectVert;
	DebugObjectVert.x = x;
	DebugObjectVert.y = y;
	DebugObjectVert.z = z;
	DebugObjectVert.color = color;
	//if (m_DebugObjectVertList.size() < 10000*3)//21840*3)
	//{
		m_DebugObjectVertList.push_back(DebugObjectVert);
	//}
}

void DebugDrawMAX::end()
{
	// only create of not refreshed
	int iDebugObjectSlot = m_iDebugObject - 65535;
	if (iDebugObjectSlot < 0 || iDebugObjectSlot > 99 || m_bDebugObjectRefreshed[iDebugObjectSlot] == true)
		return;

	// create the debug object
	if (m_iDebugObjectType == DU_DRAW_LINES)
	{
		int iVertInLineCount = m_DebugObjectVertList.size();
		makepolymesh(m_iDebugObject, iVertInLineCount);
		int v = 0;
		for (int vil = 0; vil < iVertInLineCount; vil++)
		{
			// vert making a small triangle
			float fX = m_DebugObjectVertList[vil].x;
			float fY = m_DebugObjectVertList[vil].y;
			float fZ = m_DebugObjectVertList[vil].z;
			unsigned int color = m_DebugObjectVertList[vil].color;
			float fSize = 1.0f;
			addverttomesh(m_iDebugObject, v, color, fX, fY, fZ- fSize); v++;
			addverttomesh(m_iDebugObject, v, color, fX- fSize, fY, fZ+ fSize); v++;
			addverttomesh(m_iDebugObject, v, color, fX+ fSize, fY, fZ+ fSize); v++;
		}
	}
	if (m_iDebugObjectType == DU_DRAW_TRIS)
	{
		int iVertCount = m_DebugObjectVertList.size();
		makepolymesh(m_iDebugObject, iVertCount / 3);
		for (int v = 0; v < iVertCount; v++)
		{
			// vert making up a polygon area
			addverttomesh(m_iDebugObject, v, m_DebugObjectVertList[v].color, m_DebugObjectVertList[v].x, m_DebugObjectVertList[v].y, m_DebugObjectVertList[v].z);
		}
	}
	finalisepolymesh(m_iDebugObject);

	// mark as refreshed
	m_bDebugObjectRefreshed[iDebugObjectSlot] = true;
	//glEnd();
	//glLineWidth(1.0f);
	//glPointSize(1.0f);
}

void DebugDrawMAX::init()
{
	// reset debug object vars and slots
	m_iDebugObject = 65535;
	for (int s = 0; s < 100; s++)
	{
		m_bDebugObjectRefreshed[s] = false;
	}
}

void DebugDrawMAX::setDebugObjectSlot(int iSlot, bool bNeedRefresh)
{
	// reset debug object vars and slots
	if (iSlot >= 0 && iSlot <= 99)
	{
		if (bNeedRefresh == true) m_bDebugObjectRefreshed[iSlot] = false;
		m_iDebugObject = 65535 + iSlot;
	}
}

void DebugDrawMAX::hideDebugObjects(void)
{
	for (int iSlot = 0; iSlot <= 99; iSlot++)
	{
		m_bDebugObjectRefreshed[iSlot] = false;
		m_iDebugObject = 65535 + iSlot;
		extern int ObjectExist(int iObj);
		if (ObjectExist(m_iDebugObject) == 1)
		{
			extern void DeleteObject(int iObj);
			DeleteObject(m_iDebugObject);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FileIO::FileIO() :
	m_fp(0),
	m_mode(-1)
{
}

FileIO::~FileIO()
{
	if (m_fp) fclose(m_fp);
}

bool FileIO::openForWrite(const char* path)
{
	if (m_fp) return false;
	m_fp = fopen(path, "wb");
	if (!m_fp) return false;
	m_mode = 1;
	return true;
}

bool FileIO::openForRead(const char* path)
{
	if (m_fp) return false;
	m_fp = fopen(path, "rb");
	if (!m_fp) return false;
	m_mode = 2;
	return true;
}

bool FileIO::isWriting() const
{
	return m_mode == 1;
}

bool FileIO::isReading() const
{
	return m_mode == 2;
}

bool FileIO::write(const void* ptr, const size_t size)
{
	if (!m_fp || m_mode != 1) return false;
	fwrite(ptr, size, 1, m_fp);
	return true;
}

bool FileIO::read(void* ptr, const size_t size)
{
	if (!m_fp || m_mode != 2) return false;
	size_t readLen = fread(ptr, size, 1, m_fp);
	return readLen == 1;
}


