#include "ImageCtrl.h"

CImageCtrl::CImageCtrl(void)
	: CStatic(), m_pBmp(NULL), m_gdiplusToken(0), m_sizeType(sizeType::SIZE_SCALETOFIT), m_maintainAspectRatio(true), m_aspectRatio(1), m_allignmentType(allignmentType::ALLIGN_MIDDLECENTER), m_isPanMode(TRUE), m_isZoomMode(TRUE), m_bEnbleGrid(TRUE),
	m_pntFindMarkingPnts(CPoint(-1, -1)), m_bEnbleShapeDrawing(TRUE)
{
	GdiplusStartupInput gdiplusStartupInput; GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	m_isImageShown = FALSE; m_panAtPt.SetPoint(DEAFALUT_AVOID_VAL, DEAFALUT_AVOID_VAL); m_panOffset.SetPoint(0, 0); m_zoomAtPt.SetPoint(-1, -1); m_zoomFactor = 1.0; m_zoomMin = 1; m_zoomMax = 99999;
}

CImageCtrl::~CImageCtrl(void)
{
	release(false); GdiplusShutdown(m_gdiplusToken);
}

BEGIN_MESSAGE_MAP(CImageCtrl, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CImageCtrl::EnableShapeDrawing(BOOL Shape)
{
	CSingleLock sl(&m_CtriUiDraw);
	sl.Lock();
	{
		m_bEnbleShapeDrawing = Shape;
	}
	sl.Unlock();
}

void CImageCtrl::SetImage(Bitmap * Imgptr)
{
	CSingleLock sl(&m_CtriUiDraw);
	sl.Lock();
	{
		if (!m_bEnableZoomFit)
		{
			m_panAtPt.SetPoint(-1, -1);
			m_panOffset.SetPoint(0, 0);
			m_zoomAtPt.SetPoint(-1, -1);
			m_zoomFactor = 1;

			m_isInitialShow = TRUE;
		}
	

		m_pntFindMarkingPnts = CPoint(DEAFALUT_AVOID_VAL, DEAFALUT_AVOID_VAL);
		m_dShiftXinmm = m_dShiftYinmm = DEAFALUT_AVOID_VAL;
		if(m_pBmp)
		{
			delete m_pBmp; m_pBmp = NULL;
		}

		m_pBmp = Imgptr;
		Invalidate();
	}
	sl.Unlock();
}

BOOL CImageCtrl::load(BYTE* pData, size_t nSize)
{
	IStream *pStream = NULL;

	release();
	if (CreateStreamOnHGlobal(NULL, TRUE, &pStream) == S_OK)
	{
		if (pStream->Write(pData, (ULONG)nSize, NULL) == S_OK)
			if ((m_pBmp = Bitmap::FromStream(pStream)) && m_pBmp->GetLastStatus() != Status::Ok) { delete m_pBmp; m_pBmp = NULL; }
		pStream->Release();
	}

	m_isInitialShow = TRUE; Invalidate(); UpdateWindow(); m_isInitialShow = FALSE; return m_isImageShown;
}

void CImageCtrl::ClearViewData()
{
	m_panAtPt.SetPoint(-1, -1);
	m_panOffset.SetPoint(0, 0);
	m_zoomAtPt.SetPoint(-1, -1);
	m_pntFindMarkingPnts = CPoint(DEAFALUT_AVOID_VAL, DEAFALUT_AVOID_VAL);
	m_dShiftXinmm = m_dShiftYinmm = DEAFALUT_AVOID_VAL;
	m_zoomFactor = 1;
	m_isInitialShow = TRUE;

	erase();
}

void CImageCtrl::release(bool calcFrameAspectRatio)
{
	CRect rect;
	if (m_pBmp) { delete m_pBmp; m_pBmp = NULL; }
	m_panAtPt.SetPoint(-1, -1); m_panOffset.SetPoint(0, 0); m_zoomAtPt.SetPoint(-1, -1); m_zoomFactor = 1;
	if (calcFrameAspectRatio)
	{ 
		GetClientRect(rect); 
		m_aspectRatio = (double)rect.Height() / rect.Width();
	}
}

void CImageCtrl::update()
{
	m_isInitialShow = TRUE;
	Invalidate(); 
}

void CImageCtrl::erase()
{
	CDC *pDC = NULL; CRect rect;
	pDC = GetDC();
	if(pDC)
	{ 
		GetClientRect(rect);
		pDC->FillSolidRect(rect, RGB(0,0,0)); 
		CStatic::OnEraseBkgnd(pDC); 
		ReleaseDC(pDC); 
	} 
	release();
}

BOOL CImageCtrl::load(CString szFilePath)
{
	release();
	m_pBmp = Bitmap::FromFile(szFilePath);
	if (m_pBmp->GetLastStatus() != Status::Ok)
	{
		delete m_pBmp; 
		m_pBmp = NULL; 
	}

	m_isInitialShow = TRUE;
	Invalidate();
	UpdateWindow();
	return m_isImageShown;
}

BOOL CImageCtrl::load(Bitmap *imgPtr)
{
	CSingleLock sl(&m_CtriUiDraw);
	sl.Lock();
	{
		if (imgPtr->GetLastStatus() != Status::Ok)
		{
			delete m_pBmp;
			m_pBmp = NULL;
		}

		m_pBmp = imgPtr;
	}
	sl.Unlock();

	m_isInitialShow = TRUE;
	Invalidate();
	UpdateWindow();

	return m_isImageShown;
}

void CImageCtrl::PreSubclassWindow()
{
	CRect rect;
	CStatic::PreSubclassWindow();
	ModifyStyle(0, SS_OWNERDRAW | SS_NOTIFY); GetClientRect(rect); m_aspectRatio = (m_height = m_heightOriginal = rect.Height()) / (m_width = m_widthOriginal = rect.Width());
}

void CImageCtrl::OnPaint()
{
	CSingleLock sl(&m_CtriUiDraw);
	sl.Lock();
	{
		CPaintDC dc(this);
		double w0, h0, sx, sy, s, dx, dy; CRect rect; CDC dcMem; CBitmap bmpMem, *oldBmp = NULL;
		dcMem.CreateCompatibleDC(&dc);
		GetClientRect(rect);
		if (m_pBmp)
		{
			bmpMem.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()); // Create bitmap compatible with the painting device context. 
			oldBmp = dcMem.SelectObject(&bmpMem);                            // Select the bitmap into this in-memory device context.

			Graphics graphics(dcMem.m_hDC);
			dcMem.FillSolidRect(rect, RGB(0, 0, 0));

			if (m_isInitialShow)
			{
				m_widthOriginal = m_pBmp->GetWidth(); m_heightOriginal = m_pBmp->GetHeight();
				m_aspectRatio = m_heightOriginal / m_widthOriginal;

				if (!m_maintainAspectRatio)
				{
					if (m_sizeType == sizeType::SIZE_SCALETOFIT) { m_width = rect.Width(); m_height = rect.Height(); }
					else if (m_sizeType == sizeType::SIZE_ORIGINAL) { m_width = m_widthOriginal; m_height = m_heightOriginal; }
					m_left = m_top = 0;
				}
				else
				{
					if (m_sizeType == sizeType::SIZE_SCALETOFIT)
					{
						sx = rect.Width() / m_widthOriginal; sy = rect.Height() / m_heightOriginal;
						s = (sx > sy) ? sy : sx; m_height = m_aspectRatio * (m_width = s * m_widthOriginal);
					}
					else if (m_sizeType == sizeType::SIZE_CUSTOM)
					{
						sx = m_width / m_widthOriginal; sy = m_height / m_heightOriginal;
						s = (sx > sy) ? sy : sx; m_height = m_aspectRatio * (m_width = s * m_widthOriginal);
					}
					else if (m_sizeType == sizeType::SIZE_ORIGINAL) { m_width = m_widthOriginal; m_height = m_heightOriginal; }
				}

				if (m_allignmentType == allignmentType::ALLIGN_TOPLEFT) m_left = m_top = 0;
				else if (m_allignmentType == allignmentType::ALLIGN_TOPCENTER) { m_left = (rect.Width() - m_width) / 2; m_top = 0; }
				else if (m_allignmentType == allignmentType::ALLIGN_TOPRIGHT) { m_left = rect.Width() - m_width; m_top = 0; }
				else if (m_allignmentType == allignmentType::ALLIGN_MIDDLELEFT) { m_left = 0; m_top = (rect.Height() - m_height) / 2; }
				else if (m_allignmentType == allignmentType::ALLIGN_MIDDLECENTER) { m_left = (rect.Width() - m_width) / 2; m_top = (rect.Height() - m_height) / 2; }
				else if (m_allignmentType == allignmentType::ALLIGN_MIDDLERIGHT) { m_left = rect.Width() - m_width; m_top = (rect.Height() - m_height) / 2; }
				else if (m_allignmentType == allignmentType::ALLIGN_BOTTOMLEFT) { m_left = 0; m_top = rect.Height() - m_height; }
				else if (m_allignmentType == allignmentType::ALLIGN_BOTTOMCENTER) { m_left = (rect.Width() - m_width) / 2; m_top = rect.Height() - m_height; }
				else if (m_allignmentType == allignmentType::ALLIGN_BOTTOMRIGHT) { m_left = rect.Width() - m_width; m_top = rect.Height() - m_height; }
				
				m_isInitialShow = FALSE;
			}
			else if (m_zoomAtPt.x < 0) { m_left += m_panOffset.x; m_top += m_panOffset.y; } // PAN action.
			else if (m_zoomFactor > 1e-6)                                                   // ZOOM action.
			{
				ScreenToClient(&m_zoomAtPt);
				if ((dx = (m_zoomAtPt.x - m_left)) < 1e-6) { m_zoomAtPt.x = (LONG)m_left; dx = 0; }
				else if (m_zoomAtPt.x > m_left + m_width - 1e-6) { m_zoomAtPt.x = (LONG)(m_left + (dx = m_width)); }
				if ((dy = (m_zoomAtPt.y - m_top)) < 1e-6) { m_zoomAtPt.y = (LONG)m_top; dy = 0; }
				else if (m_zoomAtPt.y > m_top + m_height - 1e-6) { m_zoomAtPt.y = (LONG)(m_top + (dy = m_height)); }

				w0 = m_width * m_zoomFactor; h0 = m_height * m_zoomFactor;
				if (w0 >= m_zoomMin && w0 <= m_zoomMax && h0 >= m_zoomMin && h0 <= m_zoomMax) // "ZOOM OUT" rectangle can't have a side smaller than m_ZoomMin pixels, "ZOOM iN" can't have a side greater than m_zoomMax pixels.
				{
					dx *= (w0 / m_width); dy *= (h0 / m_height);
					m_left = m_zoomAtPt.x - dx; m_top = m_zoomAtPt.y - dy; m_height = m_aspectRatio * (m_width = w0);
					GetParent()->SendMessage(WM_APP + 1, 12345, 0); // Send message to parent window to signalize ZOOM action event.
				}
			}
			double dXratio = m_widthOriginal / m_width;
			double dYratio = m_heightOriginal / m_height;

			m_isImageShown = (graphics.DrawImage(m_pBmp, (int)(m_left + 1e-6), (int)(m_top + 1e-6), (int)(m_width + 1e-6), (int)(m_height + 1e-6)) == Status::Ok);
			
			//Drawing Border Line For Control
			Pen *BluePen = new Pen(Color(255, 0, 0, 255), 3);
			graphics.DrawRectangle(BluePen, rect.left+1, rect.top+1, rect.Width()-3, rect.Height()-3);
			delete BluePen;

			//Drawing Grid
			//DrawGridOnView(&dcMem, rect);

			//Drawing Find Center Point By Algorithm
			//DrawPlusMarkOnFindPoint(&dcMem, dXratio, dYratio);

			//Drawing Shape
			//DrawFindShape(&dcMem, dXratio, dYratio);

			//Draw Result On View
			//DrawResultTextOnView(&dcMem);

			dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY); // Copy a portion of the in-memory device context to the painting device context.
			dcMem.SelectObject(oldBmp); bmpMem.DeleteObject(); dcMem.DeleteDC();
		}
		else
		{
			dc.FillSolidRect(rect, RGB(0, 0, 0));
			m_isImageShown = FALSE;
		}

		ReleaseDC(&dc); m_zoomAtPt.SetPoint(-1, -1);
	}
	sl.Unlock();
}

void CImageCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_isImageShown && m_isPanMode)
	{
		SetCapture();
		m_panAtPt = point;
		m_panOffset.SetPoint(0, 0);

	}
}

void CImageCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_isImageShown && m_isPanMode)
	{
		m_panOffset = point - m_panAtPt;
		m_panAtPt = point; 
		Invalidate(); ;
	} 
	ReleaseCapture();
}

void CImageCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_isImageShown && m_isPanMode && nFlags & MK_LBUTTON)
	{
		m_panOffset = point - m_panAtPt; 
		m_panAtPt = point;  Invalidate();
	}
	SetFocus();
}

BOOL CImageCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_isImageShown && m_isZoomMode)
	{ 
		m_isInitialShow = FALSE; 
		m_zoomAtPt = pt;
		m_zoomFactor = 1 + (zDelta / 1200.0); Invalidate(); 
		return TRUE;
	}
	else
		return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

void  CImageCtrl::DrawGridOnView(CDC *dcMem, CRect rct)
{
	if (m_bEnbleGrid)
	{
		CPoint pntCentr = rct.CenterPoint();
		CPoint pntLeftPnt = CPoint(pntCentr.x - rct.Width() / 2, pntCentr.y);
		CPoint pntRightPnt = CPoint(pntCentr.x + rct.Width() / 2, pntCentr.y);
		CPoint pntTopPnt = CPoint(pntCentr.x, pntCentr.y - rct.Height() / 2);
		CPoint pntBtmPnt = CPoint(pntCentr.x, pntCentr.y + rct.Height() / 2);
		CPen pen, *oldpen;
		pen.CreatePen(PS_DOT, 0, RGB(255, 255, 0));
		oldpen = dcMem->SelectObject(&pen);
		dcMem->MoveTo(pntLeftPnt);
		dcMem->LineTo(pntRightPnt);
		dcMem->MoveTo(pntTopPnt);
		dcMem->LineTo(pntBtmPnt);
		dcMem->SelectObject(oldpen);
	}
}

void CImageCtrl::DrawPlusMarkOnFindPoint(CDC *dcMem, double dXratio, double dYratio)
{
	CSingleLock sl(&m_CtriUiDraw);
	sl.Lock();
	{
		if (m_pntFindMarkingPnts.x >= 0 && m_pntFindMarkingPnts.y >= 0 && m_bEnbleShapeDrawing)
		{
			CPoint pntCntrLeft;
			pntCntrLeft.x = (static_cast<LONG> ((double)m_pntFindMarkingPnts.x / dXratio + m_left)) - 10;
			pntCntrLeft.y = static_cast<LONG> ((double)m_pntFindMarkingPnts.y / dYratio + m_top);
			CPoint pntCntrRight;
			pntCntrRight.x = static_cast<LONG> (((double)m_pntFindMarkingPnts.x / dXratio + m_left)) + 10;
			pntCntrRight.y = static_cast<LONG> ((double)m_pntFindMarkingPnts.y / dYratio + m_top);
			CPoint pntCntrTop;
			pntCntrTop.x = (static_cast<LONG> ((double)m_pntFindMarkingPnts.x / dXratio + m_left));
			pntCntrTop.y = static_cast<LONG> ((double)m_pntFindMarkingPnts.y / dYratio + m_top) - 10;
			CPoint pntCntrBottom;
			pntCntrBottom.x = static_cast<LONG> (((double)m_pntFindMarkingPnts.x / dXratio + m_left));
			pntCntrBottom.y = static_cast<LONG> ((double)m_pntFindMarkingPnts.y / dYratio + m_top) + 10;

			CPen Cntrpen, *Cntroldpen;
			Cntrpen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));

			Cntroldpen = dcMem->SelectObject(&Cntrpen);
			dcMem->MoveTo(pntCntrLeft);
			dcMem->LineTo(pntCntrRight);
			dcMem->MoveTo(pntCntrTop);
			dcMem->LineTo(pntCntrBottom);
			dcMem->SelectObject(Cntroldpen);
		}
	}
	sl.Unlock();
}

void CImageCtrl::DrawFindShape(CDC *dcMem, double dXratio, double dYratio)
{
	CSingleLock sl(&m_CtriUiDraw);
	sl.Lock();
	{	if (m_pntFindMarkingPnts.x >= 0 && m_pntFindMarkingPnts.y >= 0 && m_bEnbleShapeDrawing)
		{
			Graphics graphics(dcMem->m_hDC);

			Gdiplus::Rect rect;
			rect.X = static_cast<INT>((m_pntFindMarkingPnts.x - m_lDiameterOfFindMark / 2)/ dXratio+m_left) ;
			rect.Y = static_cast<INT>((m_pntFindMarkingPnts.y - m_lDiameterOfFindMark / 2)/ dYratio+m_top);
			rect.Width = static_cast<INT>(m_lDiameterOfFindMark/ dXratio) ;
			rect.Height = static_cast<INT>(m_lDiameterOfFindMark/ dYratio);

			//Drawing Circle
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
			Pen *pen = new Pen(Color(255, 0, 255, 0), 2);
			pen->SetWidth(static_cast<Gdiplus::REAL>(2));
			pen->SetDashStyle(DashStyle::DashStyleDashDot);

			graphics.DrawEllipse(pen, rect);
			delete pen;
		}
	}
	sl.Unlock();
}

void  CImageCtrl::DrawResultTextOnView(CDC *dc)
{
	CSingleLock sl(&m_CtriUiDraw);
	sl.Lock();
	{
		if (m_bEnbleShapeDrawing)
		{
			SetBkMode(dc->m_hDC, TRANSPARENT);
			dc->SetTextColor(RGB(255, 0, 0));
			wstring strResut = L"";
			strResut = (m_dShiftYinmm != DEAFALUT_AVOID_VAL) ? L"x=" + to_wstring(m_dShiftXinmm) + L" mm,Y=" + to_wstring(m_dShiftYinmm) + L" mm" :
				L"x=....mm, y=....mm";
			int Size = static_cast<int>(strResut.length()) * 8;

			CRect ClientRect;
			GetClientRect(ClientRect);

			dc->TextOut(ClientRect.right - Size, ClientRect.bottom - 18, strResut.c_str());
		}
	}
	sl.Unlock();
}

void CImageCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDblClk(nFlags, point);
}

void CImageCtrl::EnableDiasblePan(bool bPan)
{
	m_isPanMode = bPan;
}

void CImageCtrl::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	CSingleLock sl(&m_CtriUiDraw);
	sl.Lock();
	{
		m_panAtPt.SetPoint(-1, -1);
		m_panOffset.SetPoint(0, 0);
		m_zoomAtPt.SetPoint(-1, -1);
		m_zoomFactor = 1;
		m_isInitialShow = TRUE;

		Invalidate();
	}sl.Unlock();
}


void CImageCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: Add your message handler code here
}

void CImageCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
	if (m_pBmp)
	{
		switch (nChar)
		{
		case VK_SPACE:
		{
			auto Parent = GetParent();
			CWnd *pWnd = GetParent();
			if (pWnd == nullptr)
				TRACE(L"Parent Is Empty");
			pWnd->PostMessage(WM_COMMAND, CMD_SAVE_VIEW_IMAGE);
		}
		break;
		default:
			break;
		}
	}
}
