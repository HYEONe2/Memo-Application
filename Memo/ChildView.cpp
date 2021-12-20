
// ChildView.cpp: CChildView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Memo.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
	if (m_TextBox)
	{
		delete m_TextBox;
	}
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_CREATE, &CChildView::OnNewCreate)
	ON_COMMAND(ID_OPEN, &CChildView::OnOpen)
	ON_COMMAND(ID_SAVE, &CChildView::OnSave)
	ON_COMMAND(ID_EDIT_COPY, &CChildView::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CChildView::OnEditPaste)
	ON_COMMAND(ID_EDIT_CUT, &CChildView::OnEditCut)
	ON_COMMAND(ID_DELETE, &CChildView::OnDelete)
	ON_COMMAND(ID_EDIT_UNDO, &CChildView::OnEditUndo)
	ON_COMMAND(ID_SELECT_ALL, &CChildView::OnSelectAll)
	ON_COMMAND(ID_TIME_DATE, &CChildView::OnTimeDate)
END_MESSAGE_MAP()



// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CWnd::OnPaint()를 호출하지 마십시오.
	InvalidateTextBox();
}



int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CRect rect;
	GetClientRect(rect);

	m_TextBox = new CEdit();
	m_TextBox->Create(WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_VSCROLL | WS_BORDER | ES_MULTILINE,
		CRect(0, 0, rect.right, rect.bottom), this, 0);

	return 0;
}

void CChildView::InvalidateTextBox()
{
	CRect editRect;
	m_TextBox->GetWindowRect(editRect);

	ScreenToClient(editRect);

	CRect clientRect;
	GetClientRect(clientRect);

	editRect.right = clientRect.right;
	editRect.bottom = clientRect.bottom;

	m_TextBox->MoveWindow(editRect);
}


void CChildView::OnNewCreate()
{
	if (!m_TextBox)
		return;

	m_TextBox->SetSel(0, -1, TRUE);
	m_TextBox->Clear();
}


void CChildView::OnOpen()
{
	TCHAR BASED_CODE szFilter[] = _T("텍스트 파일(*.TXT) | *.TXT;*.txt; | 모든파일(*.*) |*.*|");
	CFileDialog dlg(FALSE, _T("*.txt"), 0, OFN_HIDEREADONLY, szFilter);

	CString strPath, strLoad, strTotal;
	FILE* pFile;

	if (dlg.DoModal() == IDOK) {
		strPath = dlg.GetPathName();

		errno_t err = _tfopen_s(&pFile, strPath, _T("rt,ccs=UNICODE"));
		if (err != 0)
			return;

		CStdioFile m_File(pFile);
		
		while(m_File.ReadString(strLoad))
			strTotal += strLoad + _T("\n");

		if (m_TextBox)
			m_TextBox->SetWindowText(strTotal);

		m_File.Close();
	}

	if (m_TextBox)
		m_TextBox->SetSel(-1, -1);
}


void CChildView::OnSave()
{
	TCHAR BASED_CODE szFilter[] = _T("텍스트 파일(*.TXT) | *.TXT;*.txt; | 모든파일(*.*) |*.*|");
	CFileDialog dlg(FALSE, _T("*.txt"), 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	CString strPath, strTotal;
	FILE* pFile;

	if (dlg.DoModal() == IDOK) {
		strPath = dlg.GetPathName();

		errno_t err = _tfopen_s(&pFile, strPath, _T("wt,ccs=UNICODE"));
		if (err != 0)
			return;

		CStdioFile m_File(pFile);

		if (m_TextBox)
			m_TextBox->GetWindowTextW(strTotal);

		m_File.WriteString(strTotal);
		m_File.Close();
	}
}


void CChildView::OnEditCopy()
{
	if (!m_TextBox)
		return;

	m_TextBox->SendMessage(WM_COPY, 0, 0);
}


void CChildView::OnEditPaste()
{
	if (!m_TextBox)
		return;

	m_TextBox->SendMessage(WM_PASTE, 0, 0);
}


void CChildView::OnEditCut()
{
	if (!m_TextBox)
		return;

	m_TextBox->SendMessage(WM_CUT, 0, 0);
}


void CChildView::OnDelete()
{
	if (!m_TextBox)
		return;

	CString strTotal; 
	m_TextBox->GetWindowTextW(strTotal);

	int nSelStart, nSelEnd;
	int nLineNum, nColNum;

	m_TextBox->GetSel(nSelStart, nSelEnd);
	nLineNum = m_TextBox->LineFromChar(nSelStart);
	nColNum = nSelStart - m_TextBox->LineIndex(nLineNum);

	strTotal.Delete(nColNum);
	m_TextBox->SetWindowTextW(strTotal);
	m_TextBox->SetSel(nColNum, nColNum);
}


void CChildView::OnEditUndo()
{
	if (!m_TextBox)
		return;

	m_TextBox->SendMessage(WM_UNDO, 0, 0);
}


void CChildView::OnSelectAll()
{
	if (!m_TextBox)
		return;

	m_TextBox->SendMessage(EM_SETSEL, 0, -1);
}


void CChildView::OnTimeDate()
{
	if (!m_TextBox)
		return;

	CString strDate, strTime, strTotal;
	CTime Time = CTime::GetCurrentTime(); 

	bool Check = (Time.GetHour() >= 12) ? true : false;
	if (Check)
		strTime.Format(TEXT("오후 %01d:%02d "), Time.GetHour() - 12, Time.GetMinute());
	else
		strTime.Format(TEXT("오전 %01d:%02d "), Time.GetHour(), Time.GetMinute());
	
	strDate.Format(TEXT("%04d-%02d-%02d"), Time.GetYear(), Time.GetMonth(), Time.GetDay());

	strTotal.Append(strTime);
	strTotal.Append(strDate);
	
	m_TextBox->ReplaceSel(strTotal);
}