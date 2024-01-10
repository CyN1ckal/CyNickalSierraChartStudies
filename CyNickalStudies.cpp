
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

SCSFExport scsf_CBRATRTextDrawing(SCStudyGraphRef sc)
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
		sc.GraphName = "CBR/ATR Text Drawing";
		sc.StudyDescription = "Displays a text drawing showing the current CBR/ATR.";
		sc.AutoLoop = 1;
		sc.GraphRegion = 0;



		// subgraphs
		Subgraph_CBR.Name = "CBR";
		Subgraph_CBR.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_ATR.Name = "ATR";
		Subgraph_ATR.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_CBRATR.Name = "CBR / ATR";
		Subgraph_CBRATR.DrawStyle = DRAWSTYLE_IGNORE;

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

	float CBRATR = Subgraph_CBR[sc.Index] / Subgraph_ATR[sc.Index];

	Subgraph_CBRATR[sc.Index] = CBRATR;

	// setting up persistent int
	int& TextLineNumber = sc.GetPersistentInt(2);

	// clear persistent ints on recalculation
	if (sc.IsFullRecalculation && sc.Index == 0)
	{
		sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, TextLineNumber);
		TextLineNumber = 0;
	}
	// if no text drawing, create one
	if (TextLineNumber == 0)
	{
		s_UseTool TextTool;
		TextTool.Clear();

		TextTool.DrawingType = DRAWING_TEXT;
		TextTool.BeginValue = 100;
		TextTool.FontBold = true;
		TextTool.FontSize = 16;
		TextTool.Color = RGB(255, 165, 0);
		TextTool.FontBackColor = RGB(0, 0, 0);
		TextTool.UseRelativeVerticalValues = true;
		TextTool.EndDateTime = 150;
		TextTool.Text.Format("%.3g", CBRATR);
		TextTool.Text.Append(" CBR/ATR");
		TextTool.AddAsUserDrawnDrawing = 1;

		sc.UseTool(TextTool);

		TextLineNumber = TextTool.LineNumber;
	}
	else // drawing exists, update text drawing
	{
		s_UseTool TextTool;
		TextTool.Text.Format("%.3g", CBRATR);
		TextTool.Text.Append(" CBR/ATR");
		TextTool.LineNumber = TextLineNumber;
		TextTool.AddAsUserDrawnDrawing = 1;

		sc.UseTool(TextTool);
	}



}
