
#include "sierrachart.h"

SCDLLName("CyNickal's Custom Sierra Chart Studies")


SCSFExport scsf_CurrentBarRangevsAverageTrueRange(SCStudyGraphRef sc)
{
	// declare subgraphs
	SCSubgraphRef Subgraph_CBR = sc.Subgraph[0];
	SCSubgraphRef Subgraph_ATR = sc.Subgraph[1];
	SCSubgraphRef Subgraph_CBRATR = sc.Subgraph[2];

	// declare inputs
	SCInputRef Input_ATRLength = sc.Input[0];
	SCInputRef Input_ATRMAType = sc.Input[1];

	if (sc.SetDefaults)
	{
		// sc settings
		sc.GraphName = "CBR vs ATR";
		sc.StudyDescription = "This study compares the current bar's range to the ATR.";
		sc.AutoLoop = 1;
		
		// subgraphs
		Subgraph_CBR.Name = "CBR";
		Subgraph_CBR.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_ATR.Name = "ATR";
		Subgraph_ATR.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_CBRATR.Name = "CBR / ATR";
		Subgraph_CBRATR.PrimaryColor = RGB(255,255,255);

		// inputs
		Input_ATRLength.Name = "ATR Length";
		Input_ATRLength.SetInt(14);
		Input_ATRLength.SetDescription("The length of the moving average in the ATR calculation.");

		Input_ATRMAType.Name = "ATR Moving Average Type";
		Input_ATRMAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		Input_ATRMAType.SetDescription("The type of moving average to be used in ATR calculation.");


		return;
	}

	// calculating CBR, ATR, and CBR/ATR
	Subgraph_CBR[sc.Index] = (sc.High[sc.Index] - sc.Low[sc.Index]);
	sc.ATR(sc.BaseDataIn, Subgraph_ATR, Input_ATRLength.GetInt(), Input_ATRMAType.GetMovAvgType());
	Subgraph_CBRATR[sc.Index] = Subgraph_CBR[sc.Index] / Subgraph_ATR[sc.Index];

}

SCSFExport scsf_RandomEntries(SCStudyGraphRef sc)
{
	if (sc.IsFullRecalculation && sc.Index == 0)//This indicates a study is being recalculated.
	{

		// When there is a full recalculation of the study,
		// reset the persistent variables we are using
		sc.GetPersistentInt(0) = 0;				// persistent second

	}

	// declare inputs
	SCInputRef Input_EnterChance= sc.Input[0];
	SCInputRef Input_ExitChance = sc.Input[1];

	// declare subgraphs
	SCSubgraphRef Subgraph_SecondsDisplay = sc.Subgraph[0];
	SCSubgraphRef Subgraph_LastSecondChecked = sc.Subgraph[1];
	SCSubgraphRef Subgraph_EnterChance = sc.Subgraph[2];
	SCSubgraphRef Subgraph_ExitChance = sc.Subgraph[3];


	if (sc.SetDefaults)
	{
		// sc settings
		sc.GraphName = "Random Entries";
		sc.StudyDescription = "This study will randomly enter and exit trades based on criteria you input.";
		sc.AutoLoop = 1;
		sc.AllowOnlyOneTradePerBar = 0;
		sc.AllowMultipleEntriesInSameDirection = 1;
		sc.MaximumPositionAllowed = 100;

		// inputs
		Input_EnterChance.Name = "1/x Chance To Enter Trade Per Second";
		Input_EnterChance.SetInt(15);
		Input_EnterChance.SetIntLimits(0, INT_MAX);

		Input_ExitChance.Name = "1/x Chance To Exit Trade Per Second";
		Input_ExitChance.SetInt(60);
		Input_ExitChance.SetIntLimits(0, INT_MAX);

		// subgraphs
		Subgraph_SecondsDisplay.Name = "Current Second";
		Subgraph_SecondsDisplay.DrawStyle = DRAWSTYLE_HIDDEN;

		Subgraph_EnterChance.Name = "Entry Chance";
		Subgraph_EnterChance.DrawStyle = DRAWSTYLE_HIDDEN;

		Subgraph_ExitChance.Name = "Exit Chance";
		Subgraph_ExitChance.DrawStyle = DRAWSTYLE_HIDDEN;
		
		Subgraph_LastSecondChecked.Name = "Last Second Checked";
		Subgraph_LastSecondChecked.DrawStyle = DRAWSTYLE_HIDDEN;

		return;
	}


	// making DateTime and getting time in seconds
	SCDateTime DateTime = sc.LatestDateTimeForLastBar;
	int seconds = DateTime.GetSecond();
	
	// setting persistent int
	int& PersistentSeconds = sc.GetPersistentInt(0);

	// setting subgraph displays
	Subgraph_SecondsDisplay[sc.Index] = seconds;
	Subgraph_EnterChance[sc.Index] = Input_EnterChance.GetInt();
	Subgraph_ExitChance[sc.Index] = Input_ExitChance.GetInt();
	Subgraph_LastSecondChecked[sc.Index] = PersistentSeconds;

	if (PersistentSeconds != seconds) // should be once per second.
	{
		// set persistent second to current second
		sc.SetPersistentInt(0, seconds);



		// getting position data
		s_SCPositionData PositionData;
		sc.GetTradePosition(PositionData);
		

		if (PositionData.PositionQuantity == 0)
		{
			// 1/x logic from input.
			int r1 = rand() % (Input_EnterChance.GetInt());
			if (r1 == 0)
			{
				// go long go short logic via r1
				int r1 = rand() % 2;
				if (r1)
				{
					s_SCNewOrder newOrder;
					newOrder.OrderQuantity = 1;
					newOrder.OrderType = SCT_ORDERTYPE_MARKET;
					sc.BuyEntry(newOrder);
				}
				else
				{
					s_SCNewOrder newOrder;
					newOrder.OrderQuantity = 1;
					newOrder.OrderType = SCT_ORDERTYPE_MARKET;
					sc.SellEntry(newOrder);
				}
			}
		}

		if (PositionData.PositionQuantity != 0)
		{
			int r1 = rand() % (Input_ExitChance.GetInt());
			if (r1 == 0)
			{
				if (PositionData.PositionQuantity > 0)
				{
					s_SCNewOrder newOrder;
					newOrder.OrderQuantity = 1;
					newOrder.OrderType = SCT_ORDERTYPE_MARKET;
					sc.BuyExit(newOrder);
				}
				if (PositionData.PositionQuantity < 0) {
					s_SCNewOrder newOrder;
					newOrder.OrderQuantity = 1;
					newOrder.OrderType = SCT_ORDERTYPE_MARKET;
					sc.SellExit(newOrder);
				}





			}
		}

		// log stuff
		std::string s = std::to_string(seconds);
		char const* pchar = s.c_str();
		sc.AddMessageToLog(pchar, 1);


	}

	






}

SCSFExport scsf_TestFunction(SCStudyGraphRef sc)
{
	// declares inputs and subgraphs
	SCSubgraphRef Subgraph_Test = sc.Subgraph[0];
	SCSubgraphRef Subgraph_StaticLine = sc.Subgraph[1];

	if (sc.SetDefaults) {
		sc.GraphName = "(WIP) Test Function";
		sc.StudyDescription = "This is a function that will be used for testing. Ignore it.";
		sc.AutoLoop = 1;

		Subgraph_Test.Name = "Test";

		Subgraph_StaticLine.Name = "Static Line";

		return;
	}

	Subgraph_StaticLine[sc.Index] = 1;

	if (sc.Index % 2)
	{
		Subgraph_Test[sc.Index] = 1;
	}
	else
	{
		Subgraph_Test[sc.Index] = 0;
	}
	




}

SCSFExport scsf_CurrentBarRangevsAverageTrueRangeBarColor(SCStudyGraphRef sc)
{
	// declare subgraphs
	SCSubgraphRef Subgraph_CBR = sc.Subgraph[0];
	SCSubgraphRef Subgraph_ATR = sc.Subgraph[1];
	SCSubgraphRef Subgraph_CBRATR = sc.Subgraph[2];
	SCSubgraphRef Subgraph_AboveThreshold = sc.Subgraph[3];

	// declare inputs
	SCInputRef Input_ATRLength = sc.Input[0];
	SCInputRef Input_ATRMAType = sc.Input[1];
	SCInputRef Input_BarColorThreshold = sc.Input[2];

	if (sc.SetDefaults)
	{
		// sc settings
		sc.GraphName = "CBR/ATR Bar Color";
		sc.StudyDescription = "This study highlights a given bar if it is over a certain CBR/ATR threshold.";
		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		// subgraphs
		Subgraph_CBR.Name = "CBR";
		Subgraph_CBR.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_ATR.Name = "ATR";
		Subgraph_ATR.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_CBRATR.Name = "CBR / ATR";
		Subgraph_CBRATR.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_AboveThreshold.Name = "Above Threshold";
		Subgraph_AboveThreshold.DrawStyle = DRAWSTYLE_COLORBAR;

		// inputs
		Input_ATRLength.Name = "ATR Length";
		Input_ATRLength.SetInt(14);
		Input_ATRLength.SetDescription("The length of the moving average in the ATR calculation.");

		Input_ATRMAType.Name = "ATR Moving Average Type";
		Input_ATRMAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		Input_ATRMAType.SetDescription("The type of moving average to be used in ATR calculation.");

		Input_BarColorThreshold.Name = "Bar Color Threshold";
		Input_BarColorThreshold.SetFloatLimits(0.0, FLT_MAX);
		Input_BarColorThreshold.SetFloat(1.5);

		return;
	}

	// calculating CBR, ATR, and CBR/ATR
	Subgraph_CBR[sc.Index] = (sc.High[sc.Index] - sc.Low[sc.Index]);
	sc.ATR(sc.BaseDataIn, Subgraph_ATR, Input_ATRLength.GetInt(), Input_ATRMAType.GetMovAvgType());
	Subgraph_CBRATR[sc.Index] = Subgraph_CBR[sc.Index] / Subgraph_ATR[sc.Index];

	Subgraph_AboveThreshold[sc.Index] = Subgraph_CBRATR[sc.Index] > Input_BarColorThreshold.GetFloat();
}
