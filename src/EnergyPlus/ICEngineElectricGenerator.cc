// EnergyPlus, Copyright (c) 1996-2019, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Sustainable Energy, LLC, and other
// contributors. All rights reserved.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without the U.S. Department of Energy's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// C++ Headers
#include <cmath>

// ObjexxFCL Headers
#include <ObjexxFCL/Array.functions.hh>
#include <ObjexxFCL/Fmath.hh>

// EnergyPlus Headers
#include <EnergyPlus/BranchNodeConnections.hh>
#include <EnergyPlus/CurveManager.hh>
#include <EnergyPlus/DataHVACGlobals.hh>
#include <EnergyPlus/DataIPShortCuts.hh>
#include <EnergyPlus/DataLoopNode.hh>
#include <EnergyPlus/DataPlant.hh>
#include <EnergyPlus/FluidProperties.hh>
#include <EnergyPlus/General.hh>
#include <EnergyPlus/ICEngineElectricGenerator.hh>
#include <EnergyPlus/InputProcessing/InputProcessor.hh>
#include <EnergyPlus/NodeInputManager.hh>
#include <EnergyPlus/OutputProcessor.hh>
#include <EnergyPlus/PlantUtilities.hh>
#include <EnergyPlus/UtilityRoutines.hh>

namespace EnergyPlus {

namespace ICEngineElectricGenerator {

    // MODULE INFORMATION:
    //       AUTHOR         Dan Fisher
    //       DATE WRITTEN   Sept. 2000
    //       MODIFIED       na
    //       RE-ENGINEERED  na

    // PURPOSE OF THIS MODULE:
    // This module simulates the operation of IC ENGINE Generators.

    // METHODOLOGY EMPLOYED:
    // Once the ElectricPowerManager determines that the IC ENGINE Generator
    // is available to meet an electric load demand, it calls SimICEngineGenerator
    // which in turn calls the ICEngine Generator model.

    Real64 const ReferenceTemp(25.0); // Reference temperature by which lower heating
    // value is reported.  This should be subtracted
    // off of when calculated exhaust energies.

    int NumICEngineGenerators(0); // number of IC ENGINE Generators specified in input
    bool getICEInput(true);       // When TRUE, calls subroutine to read input file.

    // Object Data
    Array1D<ICEngineGeneratorSpecs> ICEngineGenerator; // dimension to number of machines

    void SimICEngineGenerator(int const EP_UNUSED(GeneratorType), // type of Generator
                              std::string const &GeneratorName,   // user specified name of Generator
                              int &GeneratorIndex,
                              bool const RunFlag,  // simulate Generator when TRUE
                              Real64 const MyLoad, // demand on electric generator
                              bool const FirstHVACIteration)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Dan Fisher
        //       DATE WRITTEN   Sept. 2000
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE: This is the IC ENGINE Generator model driver.  It
        // gets the input for the models, initializes simulation variables, call
        // the appropriate model and sets up reporting variables.

        int genNum; // Generator number counter

        // Get Generator data from input file
        if (getICEInput) {
            GetICEngineGeneratorInput();
            getICEInput = false;
        }

        // SELECT and CALL MODELS
        if (GeneratorIndex == 0) {
            genNum = UtilityRoutines::FindItemInList(GeneratorName, ICEngineGenerator);
            if (genNum == 0) ShowFatalError("SimICEngineGenerator: Specified Generator not one of Valid ICEngine Generators " + GeneratorName);
            GeneratorIndex = genNum;
        } else {
            genNum = GeneratorIndex;
            if (genNum > NumICEngineGenerators || genNum < 1) {
                ShowFatalError("SimICEngineGenerator: Invalid GeneratorIndex passed=" + General::TrimSigDigits(genNum) +
                               ", Number of IC Engine Generators=" + General::TrimSigDigits(NumICEngineGenerators) + ", Generator name=" + GeneratorName);
            }
            if (ICEngineGenerator(genNum).CheckEquipName) {
                if (GeneratorName != ICEngineGenerator(genNum).Name) {
                    ShowFatalError("SimICEngineGenerator: Invalid GeneratorIndex passed=" + General::TrimSigDigits(genNum) +
                                   ", Generator name=" + GeneratorName + ", stored Generator Name for that index=" + ICEngineGenerator(genNum).Name);
                }
                ICEngineGenerator(genNum).CheckEquipName = false;
            }
        }

        auto &thisICE = ICEngineGenerator(genNum);

        thisICE.InitICEngineGenerators(RunFlag, FirstHVACIteration);
        thisICE.CalcICEngineGeneratorModel(RunFlag, MyLoad);
    }

    void GetICEGeneratorResults(int const EP_UNUSED(GeneratorType), // type of Generator
                                int const GeneratorIndex,
                                Real64 &GeneratorPower,  // electrical power
                                Real64 &GeneratorEnergy, // electrical energy
                                Real64 &ThermalPower,    // heat power
                                Real64 &ThermalEnergy    // heat energy
    )
    {
        GeneratorPower = ICEngineGenerator(GeneratorIndex).ElecPowerGenerated;
        GeneratorEnergy = ICEngineGenerator(GeneratorIndex).ElecEnergyGenerated;
        ThermalPower = ICEngineGenerator(GeneratorIndex).QTotalHeatRecovered;
        ThermalEnergy = ICEngineGenerator(GeneratorIndex).TotalHeatEnergyRec;
    }

    void SimICEPlantHeatRecovery(std::string const &EP_UNUSED(CompType),
                                 std::string const &CompName,
                                 int const EP_UNUSED(CompTypeNum),
                                 int &CompNum,
                                 bool const EP_UNUSED(RunFlag),
                                 bool &InitLoopEquip,
                                 Real64 &EP_UNUSED(MyLoad),
                                 Real64 &MaxCap,
                                 Real64 &MinCap,
                                 Real64 &OptCap,
                                 bool const FirstHVACIteration // TRUE if First iteration of simulation
    )
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         BGriffith
        //       DATE WRITTEN   March 2008
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Fill data needed in PlantLoopEquipments

        if (getICEInput) {
            GetICEngineGeneratorInput();
            getICEInput = false;
        }

        if (InitLoopEquip) {
            CompNum = UtilityRoutines::FindItemInList(CompName, ICEngineGenerator);
            if (CompNum == 0) {
                ShowFatalError("SimICEPlantHeatRecovery: ICE Generator Unit not found=" + CompName);
                return;
            }
            MinCap = 0.0;
            MaxCap = 0.0;
            OptCap = 0.0;
            return;
        } // End Of InitLoopEquip

        PlantUtilities::UpdateComponentHeatRecoverySide(ICEngineGenerator(CompNum).HRLoopNum,
                                        ICEngineGenerator(CompNum).HRLoopSideNum,
                                        DataPlant::TypeOf_Generator_ICEngine,
                                        ICEngineGenerator(CompNum).HeatRecInletNodeNum,
                                        ICEngineGenerator(CompNum).HeatRecOutletNodeNum,
                                        ICEngineGenerator(CompNum).QTotalHeatRecovered,
                                                        ICEngineGenerator(CompNum).HeatRecInletTemp,
                                                        ICEngineGenerator(CompNum).HeatRecOutletTemp,
                                                        ICEngineGenerator(CompNum).HeatRecMdotActual,
                                        FirstHVACIteration);
    }

    void GetICEngineGeneratorInput()
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR:          Dan Fisher
        //       DATE WRITTEN:    Sept. 2000

        // PURPOSE OF THIS SUBROUTINE:
        // This routine will get the input
        // required by the IC ENGINE Generator models.

        int genNum;               // Generator counter
        int NumAlphas;                  // Number of elements in the alpha array
        int NumNums;                    // Number of elements in the numeric array
        int IOStat;                     // IO Status when calling get input subroutine
        Array1D_string AlphArray(10);   // character string data
        Array1D<Real64> NumArray(11);   // numeric data
        static bool ErrorsFound(false); // error flag

        // FLOW
        DataIPShortCuts::cCurrentModuleObject = "Generator:InternalCombustionEngine";
        NumICEngineGenerators = inputProcessor->getNumObjectsFound(DataIPShortCuts::cCurrentModuleObject);

        if (NumICEngineGenerators <= 0) {
            ShowSevereError("No " + DataIPShortCuts::cCurrentModuleObject + " equipment specified in input file");
            ErrorsFound = true;
        }

        // ALLOCATE ARRAYS
        ICEngineGenerator.allocate(NumICEngineGenerators);

        // LOAD ARRAYS WITH IC ENGINE Generator CURVE FIT  DATA
        for (genNum = 1; genNum <= NumICEngineGenerators; ++genNum) {
            inputProcessor->getObjectItem(DataIPShortCuts::cCurrentModuleObject,
                                          genNum,
                                          AlphArray,
                                          NumAlphas,
                                          NumArray,
                                          NumNums,
                                          IOStat,
                                          _,
                                          DataIPShortCuts::lAlphaFieldBlanks,
                                          DataIPShortCuts::cAlphaFieldNames,
                                          DataIPShortCuts::cNumericFieldNames);
            UtilityRoutines::IsNameEmpty(AlphArray(1), DataIPShortCuts::cCurrentModuleObject, ErrorsFound);

            ICEngineGenerator(genNum).Name = AlphArray(1);

            ICEngineGenerator(genNum).RatedPowerOutput = NumArray(1);
            if (NumArray(1) == 0.0) {
                ShowSevereError("Invalid " + DataIPShortCuts::cNumericFieldNames(1) + '=' + General::RoundSigDigits(NumArray(1), 2));
                ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + AlphArray(1));
                ErrorsFound = true;
            }

            // Not sure what to do with electric nodes, so do not use optional arguments
            ICEngineGenerator(genNum).ElectricCircuitNode = NodeInputManager::GetOnlySingleNode(
                AlphArray(2), ErrorsFound, DataIPShortCuts::cCurrentModuleObject, AlphArray(1), DataLoopNode::NodeType_Electric, DataLoopNode::NodeConnectionType_Electric, 1, DataLoopNode::ObjectIsNotParent);

            ICEngineGenerator(genNum).MinPartLoadRat = NumArray(2);
            ICEngineGenerator(genNum).MaxPartLoadRat = NumArray(3);
            ICEngineGenerator(genNum).OptPartLoadRat = NumArray(4);

            // Load Special IC ENGINE Generator Curve Fit Inputs
            ICEngineGenerator(genNum).ElecOutputFuelCurve = CurveManager::GetCurveIndex(AlphArray(3)); // convert curve name to number
            if (ICEngineGenerator(genNum).ElecOutputFuelCurve == 0) {
                ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(3) + '=' + AlphArray(3));
                ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + AlphArray(1));
                ErrorsFound = true;
            }

            ICEngineGenerator(genNum).RecJacHeattoFuelCurve = CurveManager::GetCurveIndex(AlphArray(4)); // convert curve name to number
            if (ICEngineGenerator(genNum).RecJacHeattoFuelCurve == 0) {
                ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(4) + '=' + AlphArray(4));
                ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + AlphArray(1));
                ErrorsFound = true;
            }

            ICEngineGenerator(genNum).RecLubeHeattoFuelCurve = CurveManager::GetCurveIndex(AlphArray(5)); // convert curve name to number
            if (ICEngineGenerator(genNum).RecLubeHeattoFuelCurve == 0) {
                ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(5) + '=' + AlphArray(5));
                ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + AlphArray(1));
                ErrorsFound = true;
            }

            ICEngineGenerator(genNum).TotExhausttoFuelCurve = CurveManager::GetCurveIndex(AlphArray(6)); // convert curve name to number
            if (ICEngineGenerator(genNum).TotExhausttoFuelCurve == 0) {
                ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(6) + '=' + AlphArray(6));
                ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + AlphArray(1));
                ErrorsFound = true;
            }

            ICEngineGenerator(genNum).ExhaustTempCurve = CurveManager::GetCurveIndex(AlphArray(7)); // convert curve name to number
            if (ICEngineGenerator(genNum).ExhaustTempCurve == 0) {
                ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(7) + '=' + AlphArray(7));
                ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + AlphArray(1));
                ErrorsFound = true;
            } else {
                Real64 xValue = CurveManager::CurveValue(ICEngineGenerator(genNum).ExhaustTempCurve, 1.0);
                if (xValue < ReferenceTemp) {
                    ShowSevereError("GetICEngineGeneratorInput: " + DataIPShortCuts::cAlphaFieldNames(7) + " output has very low value.");
                    ShowContinueError("...curve generates [" + General::RoundSigDigits(xValue, 3) + " C] at PLR=1.0");
                    ShowContinueError("...this is less than the Reference Temperature [" + General::RoundSigDigits(ReferenceTemp, 2) +
                                      " C] and may cause errors.");
                }
            }

            ICEngineGenerator(genNum).UACoef(1) = NumArray(5);
            ICEngineGenerator(genNum).UACoef(2) = NumArray(6);

            ICEngineGenerator(genNum).MaxExhaustperPowerOutput = NumArray(7);
            ICEngineGenerator(genNum).DesignMinExitGasTemp = NumArray(8);
            ICEngineGenerator(genNum).FuelHeatingValue = NumArray(9);
            ICEngineGenerator(genNum).DesignHeatRecVolFlowRate = NumArray(10);
            if (ICEngineGenerator(genNum).DesignHeatRecVolFlowRate > 0.0) {
                ICEngineGenerator(genNum).HeatRecActive = true;
                ICEngineGenerator(genNum).HeatRecInletNodeNum = NodeInputManager::GetOnlySingleNode(
                    AlphArray(8), ErrorsFound, DataIPShortCuts::cCurrentModuleObject, AlphArray(1), DataLoopNode::NodeType_Water, DataLoopNode::NodeConnectionType_Inlet, 1, DataLoopNode::ObjectIsNotParent);
                if (ICEngineGenerator(genNum).HeatRecInletNodeNum == 0) {
                    ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(8) + '=' + AlphArray(8));
                    ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + AlphArray(1));
                    ErrorsFound = true;
                }
                ICEngineGenerator(genNum).HeatRecOutletNodeNum = NodeInputManager::GetOnlySingleNode(
                    AlphArray(9), ErrorsFound, DataIPShortCuts::cCurrentModuleObject, AlphArray(1), DataLoopNode::NodeType_Water, DataLoopNode::NodeConnectionType_Outlet, 1, DataLoopNode::ObjectIsNotParent);
                if (ICEngineGenerator(genNum).HeatRecOutletNodeNum == 0) {
                    ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(9) + '=' + AlphArray(9));
                    ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + AlphArray(1));
                    ErrorsFound = true;
                }
                BranchNodeConnections::TestCompSet(DataIPShortCuts::cCurrentModuleObject, AlphArray(1), AlphArray(8), AlphArray(9), "Heat Recovery Nodes");
                PlantUtilities::RegisterPlantCompDesignFlow(ICEngineGenerator(genNum).HeatRecInletNodeNum,
                                            ICEngineGenerator(genNum).DesignHeatRecVolFlowRate);
            } else {
                ICEngineGenerator(genNum).HeatRecActive = false;
                ICEngineGenerator(genNum).HeatRecInletNodeNum = 0;
                ICEngineGenerator(genNum).HeatRecOutletNodeNum = 0;
                if (!DataIPShortCuts::lAlphaFieldBlanks(8) || !DataIPShortCuts::lAlphaFieldBlanks(9)) {
                    ShowWarningError("Since Design Heat Flow Rate = 0.0, Heat Recovery inactive for " + DataIPShortCuts::cCurrentModuleObject + '=' + AlphArray(1));
                    ShowContinueError("However, Node names were specified for Heat Recovery inlet or outlet nodes");
                }
            }

            // Fuel Type Case Statement
            {
                auto const SELECT_CASE_var(AlphArray(10));
                if (is_blank(SELECT_CASE_var)) { // If blank then the default is Diesel
                    ICEngineGenerator(genNum).FuelType = "Diesel";

                } else if ((SELECT_CASE_var == "GAS") || (SELECT_CASE_var == "NATURALGAS") || (SELECT_CASE_var == "NATURAL GAS")) {
                    ICEngineGenerator(genNum).FuelType = "Gas";

                } else if (SELECT_CASE_var == "DIESEL") {
                    ICEngineGenerator(genNum).FuelType = "Diesel";

                } else if (SELECT_CASE_var == "GASOLINE") {
                    ICEngineGenerator(genNum).FuelType = "Gasoline";

                } else if ((SELECT_CASE_var == "FUEL OIL #1") || (SELECT_CASE_var == "FUELOIL#1") || (SELECT_CASE_var == "FUEL OIL") ||
                           (SELECT_CASE_var == "DISTILLATE OIL")) {
                    ICEngineGenerator(genNum).FuelType = "FuelOil#1";

                } else if ((SELECT_CASE_var == "FUEL OIL #2") || (SELECT_CASE_var == "FUELOIL#2") || (SELECT_CASE_var == "RESIDUAL OIL")) {
                    ICEngineGenerator(genNum).FuelType = "FuelOil#2";

                } else if ((SELECT_CASE_var == "PROPANE") || (SELECT_CASE_var == "LPG") || (SELECT_CASE_var == "PROPANEGAS") ||
                           (SELECT_CASE_var == "PROPANE GAS")) {
                    ICEngineGenerator(genNum).FuelType = "Propane";

                } else if (SELECT_CASE_var == "OTHERFUEL1") {
                    ICEngineGenerator(genNum).FuelType = "OtherFuel1";

                } else if (SELECT_CASE_var == "OTHERFUEL2") {
                    ICEngineGenerator(genNum).FuelType = "OtherFuel2";

                } else {
                    ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(10) + '=' + AlphArray(10));
                    ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + AlphArray(1));
                    ErrorsFound = true;
                }
            }

            ICEngineGenerator(genNum).HeatRecMaxTemp = NumArray(11);
        }

        if (ErrorsFound) {
            ShowFatalError("Errors found in processing input for " + DataIPShortCuts::cCurrentModuleObject);
        }

        for (genNum = 1; genNum <= NumICEngineGenerators; ++genNum) {
            SetupOutputVariable("Generator Produced Electric Power",
                                OutputProcessor::Unit::W,
                                ICEngineGenerator(genNum).ElecPowerGenerated,
                                "System",
                                "Average",
                                ICEngineGenerator(genNum).Name);
            SetupOutputVariable("Generator Produced Electric Energy",
                                OutputProcessor::Unit::J,
                                ICEngineGenerator(genNum).ElecEnergyGenerated,
                                "System",
                                "Sum",
                                ICEngineGenerator(genNum).Name,
                                _,
                                "ElectricityProduced",
                                "COGENERATION",
                                _,
                                "Plant");

            SetupOutputVariable("Generator " + ICEngineGenerator(genNum).FuelType + " Rate",
                                OutputProcessor::Unit::W,
                                ICEngineGenerator(genNum).FuelEnergyUseRate,
                                "System",
                                "Average",
                                ICEngineGenerator(genNum).Name);
            SetupOutputVariable("Generator " + ICEngineGenerator(genNum).FuelType + " Energy",
                                OutputProcessor::Unit::J,
                                ICEngineGenerator(genNum).FuelEnergy,
                                "System",
                                "Sum",
                                ICEngineGenerator(genNum).Name,
                                _,
                                ICEngineGenerator(genNum).FuelType,
                                "COGENERATION",
                                _,
                                "Plant");

            //    general fuel use report to match other generators.
            SetupOutputVariable("Generator Fuel HHV Basis Rate",
                                OutputProcessor::Unit::W,
                                ICEngineGenerator(genNum).FuelEnergyUseRate,
                                "System",
                                "Average",
                                ICEngineGenerator(genNum).Name);
            SetupOutputVariable("Generator Fuel HHV Basis Energy",
                                OutputProcessor::Unit::J,
                                ICEngineGenerator(genNum).FuelEnergy,
                                "System",
                                "Sum",
                                ICEngineGenerator(genNum).Name);

            SetupOutputVariable("Generator " + ICEngineGenerator(genNum).FuelType + " Mass Flow Rate",
                                OutputProcessor::Unit::kg_s,
                                ICEngineGenerator(genNum).FuelMdot,
                                "System",
                                "Average",
                                ICEngineGenerator(genNum).Name);

            SetupOutputVariable("Generator Exhaust Air Temperature",
                                OutputProcessor::Unit::C,
                                ICEngineGenerator(genNum).ExhaustStackTemp,
                                "System",
                                "Average",
                                ICEngineGenerator(genNum).Name);

            if (ICEngineGenerator(genNum).HeatRecActive) {
                SetupOutputVariable("Generator Heat Recovery Mass Flow Rate",
                                    OutputProcessor::Unit::kg_s,
                                    ICEngineGenerator(genNum).HeatRecMdotActual,
                                    "System",
                                    "Average",
                                    ICEngineGenerator(genNum).Name);

                SetupOutputVariable("Generator Jacket Heat Recovery Rate",
                                    OutputProcessor::Unit::W,
                                    ICEngineGenerator(genNum).QJacketRecovered,
                                    "System",
                                    "Average",
                                    ICEngineGenerator(genNum).Name);
                SetupOutputVariable("Generator Jacket Heat Recovery Energy",
                                    OutputProcessor::Unit::J,
                                    ICEngineGenerator(genNum).JacketEnergyRec,
                                    "System",
                                    "Sum",
                                    ICEngineGenerator(genNum).Name,
                                    _,
                                    "ENERGYTRANSFER",
                                    "HEATRECOVERY",
                                    _,
                                    "Plant");

                SetupOutputVariable("Generator Lube Heat Recovery Rate",
                                    OutputProcessor::Unit::W,
                                    ICEngineGenerator(genNum).QLubeOilRecovered,
                                    "System",
                                    "Average",
                                    ICEngineGenerator(genNum).Name);
                SetupOutputVariable("Generator Lube Heat Recovery Energy",
                                    OutputProcessor::Unit::J,
                                    ICEngineGenerator(genNum).LubeOilEnergyRec,
                                    "System",
                                    "Sum",
                                    ICEngineGenerator(genNum).Name,
                                    _,
                                    "ENERGYTRANSFER",
                                    "HEATRECOVERY",
                                    _,
                                    "Plant");

                SetupOutputVariable("Generator Exhaust Heat Recovery Rate",
                                    OutputProcessor::Unit::W,
                                    ICEngineGenerator(genNum).QExhaustRecovered,
                                    "System",
                                    "Average",
                                    ICEngineGenerator(genNum).Name);
                SetupOutputVariable("Generator Exhaust Heat Recovery Energy",
                                    OutputProcessor::Unit::J,
                                    ICEngineGenerator(genNum).ExhaustEnergyRec,
                                    "System",
                                    "Sum",
                                    ICEngineGenerator(genNum).Name,
                                    _,
                                    "ENERGYTRANSFER",
                                    "HEATRECOVERY",
                                    _,
                                    "Plant");

                SetupOutputVariable("Generator Produced Thermal Rate",
                                    OutputProcessor::Unit::W,
                                    ICEngineGenerator(genNum).QTotalHeatRecovered,
                                    "System",
                                    "Average",
                                    ICEngineGenerator(genNum).Name);
                SetupOutputVariable("Generator Produced Thermal Energy",
                                    OutputProcessor::Unit::J,
                                    ICEngineGenerator(genNum).TotalHeatEnergyRec,
                                    "System",
                                    "Sum",
                                    ICEngineGenerator(genNum).Name);

                SetupOutputVariable("Generator Heat Recovery Inlet Temperature",
                                    OutputProcessor::Unit::C,
                                    ICEngineGenerator(genNum).HeatRecInletTemp,
                                    "System",
                                    "Average",
                                    ICEngineGenerator(genNum).Name);

                SetupOutputVariable("Generator Heat Recovery Outlet Temperature",
                                    OutputProcessor::Unit::C,
                                    ICEngineGenerator(genNum).HeatRecOutletTemp,
                                    "System",
                                    "Average",
                                    ICEngineGenerator(genNum).Name);
            }
        }
    }

    void ICEngineGeneratorSpecs::CalcICEngineGeneratorModel(bool const RunFlag, Real64 const MyLoad)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR         Dan Fisher
        //       DATE WRITTEN   Sept. 2000
        //       MODIFIED     na
        //       RE-ENGINEERED

        // PURPOSE OF THIS SUBROUTINE:
        // simulate a IC ENGINE generator using the BLAST model

        // METHODOLOGY EMPLOYED:
        // curve fit of performance data:

        Real64 const ExhaustCP(1.047); // Exhaust Gas Specific Heat (J/kg-K)
        Real64 const KJtoJ(1000.0);    // convert Kjoules to joules
        static std::string const RoutineName("CalcICEngineGeneratorModel");

        // min allowed operating frac full load
        Real64 MinPartLoadRat = this->MinPartLoadRat;

        // max allowed operating frac full load
        Real64 MaxPartLoadRat = this->MaxPartLoadRat;

        // Generator nominal capacity (W)
        Real64 RatedPowerOutput = this->RatedPowerOutput;

        // curve fit parameter
        Real64 MaxExhaustperPowerOutput = this->MaxExhaustperPowerOutput;

        // Heat Recovery Fluid Mass FlowRate (kg/s)
        Real64 HeatRecMdot;

        // Heat Recovery Fluid Inlet Temperature (C)
        Real64 HeatRecInTemp;

        if (this->HeatRecActive) {
            int HeatRecInNode = this->HeatRecInletNodeNum;
            HeatRecInTemp = DataLoopNode::Node(HeatRecInNode).Temp;
            HeatRecMdot = DataLoopNode::Node(HeatRecInNode).MassFlowRate;

        } else {
            HeatRecInTemp = 0.0;
            HeatRecMdot = 0.0;
        }

        // If no loop demand or Generator OFF, return
        if (!RunFlag) {
            this->ElecPowerGenerated = 0.0;
            this->ElecEnergyGenerated = 0.0;
            this->HeatRecInletTemp = HeatRecInTemp;
            this->HeatRecOutletTemp = HeatRecInTemp;
            this->HeatRecMdotActual = 0.0;
            this->QJacketRecovered = 0.0;
            this->QExhaustRecovered = 0.0;
            this->QLubeOilRecovered = 0.0;
            this->QTotalHeatRecovered = 0.0;
            this->JacketEnergyRec = 0.0;
            this->ExhaustEnergyRec = 0.0;
            this->LubeOilEnergyRec = 0.0;
            this->TotalHeatEnergyRec = 0.0;
            this->FuelEnergyUseRate = 0.0;
            this->FuelEnergy = 0.0;
            this->FuelMdot = 0.0;
            this->ExhaustStackTemp = 0.0;

            return;
        }

        // Generator output (W)
        Real64 ElecPowerGenerated = min(MyLoad, RatedPowerOutput);
        ElecPowerGenerated = max(ElecPowerGenerated, 0.0);

        // Generator operating part load ratio
        Real64 PLR = min(ElecPowerGenerated / RatedPowerOutput, MaxPartLoadRat);
        PLR = max(PLR, MinPartLoadRat);
        ElecPowerGenerated = PLR * RatedPowerOutput;

        // DETERMINE FUEL CONSUMED AND AVAILABLE WASTE HEAT

        // Use Curve fit to determine Fuel Energy Input.  For electric power generated in Watts, the fuel
        // energy input is calculated in J/s.  The PLBasedFuelInputCurve selects ratio of fuel flow (J/s)/power generated (J/s).
        Real64 FuelEnergyUseRate; // IC ENGINE fuel use rate (W)
        if (PLR > 0.0) {
            // (RELDC) Ratio of generator output to Fuel Energy Input
            Real64 ElecOutputFuelRat = CurveManager::CurveValue(this->ElecOutputFuelCurve, PLR);
            FuelEnergyUseRate = ElecPowerGenerated / ElecOutputFuelRat;
        } else {
            FuelEnergyUseRate = 0.0;
        }

        // Use Curve fit to determine heat recovered in the water jacket.  This curve calculates the water jacket heat recovered (J/s) by
        // multiplying the total fuel input (J/s) by the fraction of that power that could be recovered in the water jacket at that
        // particular part load.

        // (RJACDC) Ratio of Recoverable Jacket Heat to Fuel Energy Input
        Real64 RecJacHeattoFuelRat = CurveManager::CurveValue(this->RecJacHeattoFuelCurve, PLR);

        // water jacket heat recovered (W)
        Real64 QJacketRec = FuelEnergyUseRate * RecJacHeattoFuelRat;

        // Use Curve fit to determine Heat Recovered Lubricant heat.  This curve calculates the lube heat recovered (J/s) by
        // multiplying the total fuel input (J/s) by the fraction of that power that could be recovered in the lube oil at that
        // particular part load.
        // (RLUBDC) Ratio of Recoverable Lube Oil Heat to Fuel Energy Input
        Real64 RecLubeHeattoFuelRat = CurveManager::CurveValue(this->RecLubeHeattoFuelCurve, PLR);

        // lube oil cooler heat recovered (W)
        Real64 QLubeOilRec = FuelEnergyUseRate * RecLubeHeattoFuelRat;

        // Use Curve fit to determine Heat Recovered from the exhaust.  This curve calculates the  heat recovered (J/s) by
        // multiplying the total fuel input (J/s) by the fraction of that power that could be recovered in the exhaust at that
        // particular part load.

        // (REXDC) Total Exhaust Energy Input to Fuel Energy Input
        Real64 TotExhausttoFuelRat = CurveManager::CurveValue(this->TotExhausttoFuelCurve, PLR);

        // total engine exhaust heat (W)
        Real64 QExhaustTotal = FuelEnergyUseRate * TotExhausttoFuelRat;

        // exhaust gas heat recovered (W)
        Real64 QExhaustRec;

        // engine stack temp. (C)
        Real64 ExhaustStackTemp = 0.0;

        // Use Curve fit to determine Exhaust Temperature in C.  The temperature is simply a curve fit
        // of the exhaust temperature in C to the part load ratio.
        if (PLR > 0.0) {
            // (TEX) Exhaust Gas Temp
            Real64 ExhaustTemp = CurveManager::CurveValue(this->ExhaustTempCurve, PLR);

            if (ExhaustTemp > ReferenceTemp) {

                // exhaust gas mass flow rate (kg/s)
                Real64 ExhaustGasFlow = QExhaustTotal / (ExhaustCP * (ExhaustTemp - ReferenceTemp));

                // Use Curve fit to determine stack temp after heat recovery
                // (UACDC) exhaust gas Heat Exchanger UA
                Real64 UA = this->UACoef(1) * std::pow(RatedPowerOutput, this->UACoef(2));

                // design engine stact saturated steam temp. (C)
                Real64 DesignMinExitGasTemp = this->DesignMinExitGasTemp;

                ExhaustStackTemp =
                    DesignMinExitGasTemp + (ExhaustTemp - DesignMinExitGasTemp) /
                                               std::exp(UA / (max(ExhaustGasFlow, MaxExhaustperPowerOutput * RatedPowerOutput) * ExhaustCP));

                QExhaustRec = max(ExhaustGasFlow * ExhaustCP * (ExhaustTemp - ExhaustStackTemp), 0.0);
            } else {
                if (this->ErrExhaustTempIndex == 0) {
                    ShowWarningMessage("CalcICEngineGeneratorModel: " + this->TypeOf + "=\"" +
                                       this->Name + "\" low Exhaust Temperature from Curve Value");
                    ShowContinueError("...curve generated temperature=[" + General::RoundSigDigits(ExhaustTemp, 3) + " C], PLR=[" + General::RoundSigDigits(PLR, 3) +
                                      "].");
                    ShowContinueError("...simulation will continue with exhaust heat reclaim set to 0.");
                }
                ShowRecurringWarningErrorAtEnd("CalcICEngineGeneratorModel: " + this->TypeOf + "=\"" +
                                               this->Name + "\" low Exhaust Temperature continues...",
                                               this->ErrExhaustTempIndex,
                                               ExhaustTemp,
                                               ExhaustTemp,
                                               _,
                                               "[C]",
                                               "[C]");
                QExhaustRec = 0.0;
                ExhaustStackTemp = this->DesignMinExitGasTemp;
            }
        } else {
            QExhaustRec = 0.0;
            // Bug ExhaustStackTemp not set but used below
        }

        Real64 QTotalHeatRecovered = QExhaustRec + QLubeOilRec + QJacketRec;

        // When Max Temp is reached the amount of recovered heat has to be reduced.
        Real64 HRecRatio;

        if (this->HeatRecActive) {
            this->CalcICEngineGenHeatRecovery(QTotalHeatRecovered, HeatRecMdot, HRecRatio);
            QExhaustRec *= HRecRatio;
            QLubeOilRec *= HRecRatio;
            QJacketRec *= HRecRatio;
            QTotalHeatRecovered *= HRecRatio;
        } else {
            this->HeatRecInletTemp = HeatRecInTemp;
            this->HeatRecOutletTemp = HeatRecInTemp;
            this->HeatRecMdotActual = HeatRecMdot;
        }

        // Calculate Energy
        // Generator output (J)
        Real64 ElectricEnergyGen = ElecPowerGenerated * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;

        // IC ENGINE fuel use (J)
        Real64 FuelEnergyUsed = FuelEnergyUseRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;

        // water jacket heat recovered (J)
        Real64 JacketEnergyRec = QJacketRec * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;

        // lube oil cooler heat recovered (J)
        Real64 LubeOilEnergyRec = QLubeOilRec * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;

        // exhaust gas heat recovered (J)
        Real64 ExhaustEnergyRec = QExhaustRec * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
        this->ElecPowerGenerated = ElecPowerGenerated;
        this->ElecEnergyGenerated = ElectricEnergyGen;
        this->QJacketRecovered = QJacketRec;
        this->QLubeOilRecovered = QLubeOilRec;
        this->QExhaustRecovered = QExhaustRec;
        this->QTotalHeatRecovered = QTotalHeatRecovered;
        this->JacketEnergyRec = JacketEnergyRec;
        this->LubeOilEnergyRec = LubeOilEnergyRec;
        this->ExhaustEnergyRec = ExhaustEnergyRec;
        this->QTotalHeatRecovered = (QExhaustRec + QLubeOilRec + QJacketRec);
        this->TotalHeatEnergyRec = (ExhaustEnergyRec + LubeOilEnergyRec + JacketEnergyRec);
        this->FuelEnergyUseRate = std::abs(FuelEnergyUseRate);
        this->FuelEnergy = std::abs(FuelEnergyUsed);

        // Heating Value of Fuel in kJ/kg
        Real64 FuelHeatingValue = this->FuelHeatingValue;

        this->FuelMdot = std::abs(FuelEnergyUseRate) / (FuelHeatingValue * KJtoJ);
        this->ExhaustStackTemp = ExhaustStackTemp;

        if (this->HeatRecActive) {
            int HeatRecOutletNode = this->HeatRecOutletNodeNum;
            DataLoopNode::Node(HeatRecOutletNode).Temp = this->HeatRecOutletTemp;
        }
    }

    void ICEngineGeneratorSpecs::CalcICEngineGenHeatRecovery(Real64 const EnergyRecovered, Real64 const HeatRecMdot, Real64 &HRecRatio)
    {
        // SUBROUTINE INFORMATION:
        //       AUTHOR:          Brandon Anderson
        //       DATE WRITTEN:    November 2000

        // PURPOSE OF THIS SUBROUTINE:
        // To perform heat recovery calculations and node updates

        // METHODOLOGY EMPLOYED: This routine is required for the heat recovery loop.
        // It works in conjunction with the Heat Recovery Manager, and the PlantWaterHeater.
        // The chiller sets the flow on the loop first by the input design flowrate and then
        // performs a check to verify that

        static std::string const RoutineName("CalcICEngineGeneratorModel");

        // Load inputs to local structure
        int HeatRecInNode = this->HeatRecInletNodeNum;

        // Need to set the HeatRecRatio to 1.0 if it is not modified
        HRecRatio = 1.0;

        Real64 HeatRecInTemp = DataLoopNode::Node(HeatRecInNode).Temp;
        Real64 HeatRecCp = FluidProperties::GetSpecificHeatGlycol(DataPlant::PlantLoop(this->HRLoopNum).FluidName,
                                          HeatRecInTemp,
                                          DataPlant::PlantLoop(this->HRLoopNum).FluidIndex,
                                          RoutineName);

        // Don't divide by zero - Note This also results in no heat recovery when
        //  design Mdot for Heat Recovery - Specified on Chiller Input - is zero
        //  In order to see what minimum heat recovery flow rate is for the design temperature
        //  The design heat recovery flow rate can be set very small, but greater than zero.

        Real64 HeatRecOutTemp;

        if ((HeatRecMdot > 0) && (HeatRecCp > 0)) {
            HeatRecOutTemp = (EnergyRecovered) / (HeatRecMdot * HeatRecCp) + HeatRecInTemp;
        } else {
            HeatRecOutTemp = HeatRecInTemp;
        }

        // Note: check to make sure the Max Temperature was not exceeded
        if (HeatRecOutTemp > this->HeatRecMaxTemp) {
            Real64 MinHeatRecMdot;
            if (this->HeatRecMaxTemp != HeatRecInTemp) {
                MinHeatRecMdot = (EnergyRecovered) / (HeatRecCp * (this->HeatRecMaxTemp - HeatRecInTemp));
                if (MinHeatRecMdot < 0.0) MinHeatRecMdot = 0.0;
            } else {
                MinHeatRecMdot = 0.0;
            }

            // Recalculate Outlet Temperature, with adjusted flowrate
            if ((MinHeatRecMdot > 0.0) && (HeatRecCp > 0.0)) {
                HeatRecOutTemp = (EnergyRecovered) / (MinHeatRecMdot * HeatRecCp) + HeatRecInTemp;
                HRecRatio = HeatRecMdot / MinHeatRecMdot;
            } else {
                HeatRecOutTemp = HeatRecInTemp;
                HRecRatio = 0.0;
            }
        }

        // Update global variables for reporting later
        this->HeatRecInletTemp = HeatRecInTemp;
        this->HeatRecOutletTemp = HeatRecOutTemp;
        this->HeatRecMdotActual = HeatRecMdot;
    }

    void ICEngineGeneratorSpecs::InitICEngineGenerators(bool const RunFlag, bool const FirstHVACIteration)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Dan Fisher
        //       DATE WRITTEN   Oct 2000
        //       MODIFIED       na
        //       RE-ENGINEERED  Brent Griffith, Sept 2010, plant upgrades, generalize fluid props

        // PURPOSE OF THIS SUBROUTINE:
        // This subroutine is for initializations of the IC ENGINE generators.

        // METHODOLOGY EMPLOYED:
        // Uses the status flags to trigger initializations.

        static std::string const RoutineName("InitICEngineGenerators");

        bool errFlag;

        if (this->MyPlantScanFlag && allocated(DataPlant::PlantLoop) && this->HeatRecActive) {
            errFlag = false;
            PlantUtilities::ScanPlantLoopsForObject(this->Name,
                                    DataPlant::TypeOf_Generator_ICEngine,
                                    this->HRLoopNum,
                                    this->HRLoopSideNum,
                                    this->HRBranchNum,
                                    this->HRCompNum,
                                    errFlag,
                                    _,
                                    _,
                                    _,
                                    _,
                                    _);
            if (errFlag) {
                ShowFatalError("InitICEngineGenerators: Program terminated due to previous condition(s).");
            }

            this->MyPlantScanFlag = false;
        }

        if (this->MySizeAndNodeInitFlag && (!this->MyPlantScanFlag) && this->HeatRecActive) {

            int HeatRecInletNode = this->HeatRecInletNodeNum;
            int HeatRecOutletNode = this->HeatRecOutletNodeNum;

            // size mass flow rate
            Real64 rho = FluidProperties::GetDensityGlycol(DataPlant::PlantLoop(this->HRLoopNum).FluidName,
                                   DataGlobals::InitConvTemp,
                                   DataPlant::PlantLoop(this->HRLoopNum).FluidIndex,
                                   RoutineName);

            this->DesignHeatRecMassFlowRate = rho * this->DesignHeatRecVolFlowRate;
            this->HeatRecMdotDesign = this->DesignHeatRecMassFlowRate;

            PlantUtilities::InitComponentNodes(0.0,
                               this->DesignHeatRecMassFlowRate,
                               HeatRecInletNode,
                               HeatRecOutletNode,
                               this->HRLoopNum,
                               this->HRLoopSideNum,
                               this->HRBranchNum,
                               this->HRCompNum);

            this->MySizeAndNodeInitFlag = false;
        } // end one time inits

        // Do the Begin Environment initializations
        if (DataGlobals::BeginEnvrnFlag && this->MyEnvrnFlag && this->HeatRecActive) {
            int HeatRecInletNode = this->HeatRecInletNodeNum;
            int HeatRecOutletNode = this->HeatRecOutletNodeNum;
            // set the node Temperature, assuming freeze control
            DataLoopNode::Node(HeatRecInletNode).Temp = 20.0;
            DataLoopNode::Node(HeatRecOutletNode).Temp = 20.0;
            // set the node max and min mass flow rates
            PlantUtilities::InitComponentNodes(0.0,
                               this->DesignHeatRecMassFlowRate,
                               HeatRecInletNode,
                               HeatRecOutletNode,
                               this->HRLoopNum,
                               this->HRLoopSideNum,
                               this->HRBranchNum,
                               this->HRCompNum);

            this->MyEnvrnFlag = false;
        } // end environmental inits

        if (!DataGlobals::BeginEnvrnFlag) {
            this->MyEnvrnFlag = true;
        }

        if (this->HeatRecActive) {
            if (FirstHVACIteration) {
                Real64 mdot;
                if (RunFlag) {
                    mdot = this->DesignHeatRecMassFlowRate;
                } else {
                    mdot = 0.0;
                }
                PlantUtilities::SetComponentFlowRate(mdot,
                                     this->HeatRecInletNodeNum,
                                     this->HeatRecOutletNodeNum,
                                     this->HRLoopNum,
                                     this->HRLoopSideNum,
                                     this->HRBranchNum,
                                     this->HRCompNum);

            } else {
                PlantUtilities::SetComponentFlowRate(this->HeatRecMdotActual,
                                     this->HeatRecInletNodeNum,
                                     this->HeatRecOutletNodeNum,
                                     this->HRLoopNum,
                                     this->HRLoopSideNum,
                                     this->HRBranchNum,
                                     this->HRCompNum);
            }
        }
    }

} // namespace ICEngineElectricGenerator

} // namespace EnergyPlus
