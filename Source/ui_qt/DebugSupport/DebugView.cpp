#include "DebugView.h"
#include "BiosDebugInfoProvider.h"
#include <QVBoxLayout>
//#include "DisAsmWnd.h"

CDebugView::CDebugView(QMdiArea* parent, CVirtualMachine& virtualMachine, CMIPS* ctx,
                       const StepFunction& stepFunction, CBiosDebugInfoProvider* biosDebugInfoProvider, const char* name, int size, CQtDisAsmTableModel::DISASM_TYPE disAsmType)
    : m_virtualMachine(virtualMachine)
    , m_ctx(ctx)
    , m_name(name)
    , m_stepFunction(stepFunction)
    , m_disAsmWnd(nullptr)
    , m_regViewWnd(nullptr)
    , m_memoryViewWnd(nullptr)
    , m_callStackWnd(nullptr)
    , m_biosDebugInfoProvider(biosDebugInfoProvider)
{

	m_disAsmWnd = new CDisAsmWnd(parent, virtualMachine, m_ctx, name, disAsmType, size);
	m_disAsmWnd->show();

	m_regViewWnd = new CRegViewWnd(parent, m_ctx);
	m_regViewWnd->show();

	m_memoryViewWnd = new CMemoryViewMIPSWnd(parent, virtualMachine, m_ctx, size);
	m_regViewWnd->show();

	m_callStackWnd = new CCallStackWnd(parent, m_ctx, m_biosDebugInfoProvider);
	m_callStackWnd->show();

	m_OnFunctionDblClickConnection = m_callStackWnd->OnFunctionDblClick.Connect(std::bind(&CDebugView::OnCallStackWndFunctionDblClick, this, std::placeholders::_1));

	Hide();
}

CDebugView::~CDebugView()
{
	delete m_disAsmWnd;
	delete m_regViewWnd;
	delete m_memoryViewWnd;
	delete m_callStackWnd;
}

void CDebugView::HandleMachineStateChange()
{
	m_disAsmWnd->HandleMachineStateChange();
	m_regViewWnd->HandleMachineStateChange();
	m_memoryViewWnd->HandleMachineStateChange();
	m_callStackWnd->HandleMachineStateChange();
}

void CDebugView::HandleRunningStateChange(CVirtualMachine::STATUS newState)
{
	m_disAsmWnd->HandleRunningStateChange(newState);
	m_regViewWnd->HandleRunningStateChange(newState);
	m_memoryViewWnd->HandleRunningStateChange(newState);
	m_callStackWnd->HandleRunningStateChange(newState);
}

const char* CDebugView::GetName() const
{
	return m_name.c_str();
}

void CDebugView::Hide()
{
	m_disAsmWnd->hide();
	m_memoryViewWnd->hide();

	m_regViewWnd->hide();
	m_callStackWnd->hide();
}

void CDebugView::Step()
{
	m_stepFunction();
}

CBiosDebugInfoProvider* CDebugView::GetBiosDebugInfoProvider() const
{
	return m_biosDebugInfoProvider;
}

CMIPS* CDebugView::GetContext()
{
	return m_ctx;
}

CDisAsmWnd* CDebugView::GetDisassemblyWindow()
{
	return m_disAsmWnd;
}

CMemoryViewMIPSWnd* CDebugView::GetMemoryViewWindow()
{
	return m_memoryViewWnd;
}

CRegViewWnd* CDebugView::GetRegisterViewWindow()
{
	return m_regViewWnd;
}

CCallStackWnd* CDebugView::GetCallStackWindow()
{
	return m_callStackWnd;
}

void CDebugView::OnCallStackWndFunctionDblClick(uint32 nAddress)
{
	auto disAsm = GetDisassemblyWindow();
	disAsm->SetCenterAtAddress(nAddress);
	disAsm->SetSelectedAddress(nAddress);
}
