// EnergyPlus, Copyright (c) 1996-2020, The Board of Trustees of the University of Illinois,
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

// Portions of the EnergyPlus(tm) software package have been developed and copyrighted by other
// individuals, companies and institutions.  These portions have been incorporated into the EnergyPlus
// software package under license.

// In addition to the primary authorship of the LBNL Simulation Research Group (
// http://simulationresearch.lbl.gov/) and the UIUC Building Systems Laboratory (
// http://www.bso.uiuc.edu/), the following have contributed to EnergyPlus V1.0:

// Portions of the EnergyPlus weather processor were developed by US Department of Energy, Building
// Technologies Program, www.energyplus.gov

// Portions of the input processing, output processing, weather processor, BLAST Translator were
// developed by US Army Corps of Engineers, Construction Engineering Research Laboratories, 2902 Newmark
// Drive, Champaign IL  61821. www.cecer.army.mil

// Portions of this software package were developed by Linda Lawrie of DHL Consulting LLC.

// Portions of this software package were developed by C.O. Pedersen Associates.

// Portions of the EnergyPlus utility software (EP-Launch, IDFEditor, DOE2Translator, HVAC-Diagram,
// ExpandObjects, CSVProc, System Templates, and convertESOMTR) were developed by GARD Analytics, Inc.
// 1028 Busse Highway, Park Ridge, Illinois 60068-1802, USA (847) 698-5690, www.gard.com.  GARD Analytics
// performed independent verification and validation testing of the software after developing the testing
// strategy and plan.  GARD Analytics was also responsible for gas absorption chiller, desiccant
// dehumidifier, ice storage (simple), table reports and economics.

// Portions of flow resolver, chiller models (absorption, electric, const cop, engine-driven, gas-
// turbine), generator models (diesel electric, gas turbine), furnace models, heat recovery loop, plant
// loop, plant condenser loop, air-change dependent inside film coefficients were developed by Oklahoma
// State University, 110 Engineering North, Stillwater, OK 74078.

// Portions of EnergyPlus related to the models for EMPD moisture calculations, DX coils, furnace/unitary
// systems, cooling towers, air-to-air heat pumps, air distribution systems, refrigerated cases, electric
// EIR chillers, packaged terminal heat pumps, desuperheater air and water heating coils, and heat pump
// water heaters were developed by University of Central Florida, Florida Solar Energy Center (FSEC),
// 1679 Clearlake Road, Cocoa, FL  32922, www.fsec.ucf.edu/.

// Portions of EnergyPlus were developed by the National Renewable Energy Laboratory (NREL), 1617 Cole
// Blvd, Golden, CO 80401.

// EnergyPlus v1.0.1, v1.0.2, v1.0.3, v1.1, v1.1.1 (Wintel platform) includes a link to TRNSYS (The Transient
// Energy System Simulation Tool) for photovoltaic calculations developed by Thermal Energy System Specialists,
// 2916 Marketplace Drive, Suite 104, Madison, WI 53719; Tel: (608) 274-2577. EnergyPlus v1.2 and later
// includes Photovoltaic calculations implemented in EnergyPlus by Thermal Energy System Specialists.
// This model was originally developed by Oystein Ulleberg, Institute for Energy Technology, Norway -- based on
// the Duffie and Beckman equivalent one-diode model.

// Portions of this software package that convert certain stand-alone heat transfer models for slab-on-
// grade and basement foundations were developed by William Bahnfleth, Cynthia Cogil, and Edward
// Clements, Department of Architectural Engineering, Pennsylvania State University, 224 Engineering Unit
// A, University Park, Pennsylvania  16802-1416, (814) 863-2076.

// The concept and initial implementation for the EnergyPlus COM/DLL version (Wintel platform) was made
// possible through cooperation with DesignBuilder Software, Ltd, Andy Tindale - an EnergyPlus
// collaborative developer.

// The thickness, conductivity, density and specific heat values of the material layers for the
// constructions in the Composite Wall Construction reference data set have been taken from the ASHRAE
// report "Modeling Two- and Three-Dimensional Heat Transfer through Composite Wall and Roof Assemblies
// in Hourly  Energy Simulation Programs (1145-TRP)," by Enermodal Engineering Limited, Oak Ridge
// National Laboratory, and the Polish Academy of Sciences, January 2001.

// EnergyPlus v1.2 contains DELight2 (wintel platform), a simulation engine for daylighting and electric
// lighting system analysis developed at Ernest Orlando Lawrence Berkeley National Laboratory.

// Portions of the EnergyPlus v1.2 air distribution system calculations were written by George Walton of
// the National Institute for Standards and Technology (NIST), 100 Bureau Drive, Gaithersburg, MD 20899,
// (301) 975-6478.  The EnergyPlus AirflowNetwork model also includes portions of an early version of COMIS
// (Conjunction Of Multizone Infiltration Specialists) developed by a multinational, multi-institutional
// effort under the auspices of the International Energy Agency's Buildings and Community Systems Agreement
// working group focusing on multizone air flow modeling (Annex 23) and now administered by the Swiss Federal
// Laboratories for Materials Testing and Research (EMPA), Division 175, Uberlandstrasse 129, CH-8600 Dubendorf,
// Switzerland.

// The EnergyPlus v1.2 model for displacement ventilation and cross-ventilation was developed
// by Guilherme Carrilho da Graca and Paul Linden of the Department of Mechanical and Aerospace
// Engineering, University of California, San Diego.

// The EnergyPlus models for UFAD served zones were developed by Anna Liu and Paul Linden at the Department
// of Mechanical and Aerospace Engineering, University of California, San Diego.

// ASHRAE research project 1254-RP supported the development of the following features first added in
// EnergyPlus v1.2.2:
//    DXSystem:AirLoop enhancements (valid as OA system equipment, new humidity control options);
//    New set point managers: SET POINT MANAGER:SINGLE ZONE HEATING, SET POINT MANAGER:SINGLE ZONE COOLING,
//            and SET POINT MANAGER:OUTSIDE AIR PRETREAT;
//    New 2-stage DX coil with enhanced dehumidification option (COIL:DX:MultiMode:CoolingEmpirical);
//    Additional DESICCANT DEHUMIDIFIER:SOLID setpoint control option;
// American Society of Heating Refrigerating and Air-Conditioning Engineers, Inc,,
// 1791 Tullie Circle, N.E., Atlanta, GA 30329. www.ashrae.org
// Work performed by GARD Analytics, Inc., 1028 Busse Highway, Park Ridge, Illinois 60068-1802, USA.
// www.gard.com, November 2004.

// EnergyPlus v1.2.2 and later versions (wintel platform) contains links to SPARK, a simulation engine for
// detailed system modeling developed at Ernest Orlando Lawrence Berkeley National Laboratory in
// conjunction with Ayres Sowell Associates, Inc.  SPARK was removed in V3.1 - April 2009 release.

// The Ecoroof (Green Roof) model, first introduced in EnergyPlus v2.0, was developed at Portland State University,
// by David Sailor and his students. It is based on the FASST vegetation models developed by Frankenstein and
// Koenig for the US Army Corps of Engineers.

// The HAMT (Heat And Moisture Transfer) model, first introduced in EnergyPlus v3.0.0 was developed by Phillip Biddulph,
// Complex Built Environment Systems, The Bartlett School of Graduate Studies, University College London, Gower Street,
// London WC1E 6BT, United Kingdom. http://www.cbes.ucl.ac.uk/.

// The SQLite output module, first introduced in EnergyPlus v3.0.0, was developed by Gregory B. Stark, P.E.,
// Building Synergies, LLC, 1860 Washington Street, Suite 208, Denver, Colorado 80203, United States.
// http://www.buildingsynergies.com/

// Refrigeration compressor performance data and refrigeration practices were provided by CDH Energy, Cazenovia, NY 12035.

// Other Acknowledgments

// This work was supported by the Assistant Secretary for Energy Efficiency and Renewable Energy, Office
// of Building Technologies Program of the US Department of Energy.

// Additional support was provided by the Gas Technology Institute and the California Energy Commission.

// The ice thermal storage module development was supported by the U.S. Department of Energy Office of
// Electricity Delivery and Energy Reliability.

// The HAMT (Heat And Moisture Transfer) model was supported by the Engineering and Physical Sciences Research Council (EPSRC),
// the UK government agency for funding research and training in engineering and the physical sciences.

// The SQLite output module was funded by Building Synergies, LLC and was made possible by inclusion of software code
// from the SQLite project (http://www.sqlite.org/).

#ifdef _WIN32
#include <Windows.h>
#endif

// C++ Headers
#include <exception>
#include <iostream>
#ifndef NDEBUG
#ifdef __unix__
#include <cfenv>
#endif
#endif

// ObjexxFCL Headers
#include <ObjexxFCL/Array1D.hh>
#include <ObjexxFCL/time.hh>

// EnergyPlus Headers
#include <EnergyPlus/CommandLineInterface.hh>
#include <EnergyPlus/DataGlobals.hh>
#include <EnergyPlus/DataStringGlobals.hh>
#include <EnergyPlus/DataSystemVariables.hh>
#include <EnergyPlus/DataTimings.hh>
#include <EnergyPlus/DisplayRoutines.hh>
#include <EnergyPlus/api/EnergyPlusPgm.hh>
#include <EnergyPlus/FileSystem.hh>
#include <EnergyPlus/FluidProperties.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/InputProcessing/DataStorage.hh>
#include <EnergyPlus/InputProcessing/IdfParser.hh>
#include <EnergyPlus/InputProcessing/InputProcessor.hh>
#include <EnergyPlus/InputProcessing/InputValidation.hh>
#include <EnergyPlus/OutputFiles.hh>
#include <EnergyPlus/OutputProcessor.hh>
#include <EnergyPlus/Psychrometrics.hh>
#include <EnergyPlus/ResultsSchema.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/SimulationManager.hh>
#include <EnergyPlus/UtilityRoutines.hh>

#ifdef _WIN32
#include <direct.h>
#include <stdlib.h>
#else // Mac or Linux
#include <unistd.h>
#endif

void EnergyPlusPgm(EnergyPlus::EnergyPlusData &state, std::string const &filepath)
{
    std::exit(RunEnergyPlus(state, filepath));
}

int initializeEnergyPlus(EnergyPlus::EnergyPlusData &state, std::string const & filepath) {
    using namespace EnergyPlus;

    // Disable C++ i/o synching with C methods for speed
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr); // Untie cin and cout: Could cause odd behavior for interactive prompts

// Enable floating point exceptions
#ifndef NDEBUG
#ifdef __unix__
    feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif
#endif

#ifdef _MSC_VER
    #ifndef _DEBUG
    // If _MSC_VER and not debug then prevent dialogs on error
    SetErrorMode(SEM_NOGPFAULTERRORBOX);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
#endif

    DataSystemVariables::Time_Start = DataTimings::epElapsedTime();
#ifdef EP_Detailed_Timings
    epStartTime("EntireRun=");
#endif

    DataStringGlobals::CurrentDateTime = CreateCurrentDateTimeString();

    ResultsFramework::OutputSchema->SimulationInformation.setProgramVersion(DataStringGlobals::VerString);
    ResultsFramework::OutputSchema->SimulationInformation.setStartDateTimeStamp(DataStringGlobals::CurrentDateTime.substr(5));

    DataStringGlobals::VerString += "," + DataStringGlobals::CurrentDateTime;

    DataSystemVariables::processEnvironmentVariables(state.dataGlobals);

    if (!filepath.empty()) {
        // if filepath is not empty, then we are using E+ as a library API call
        // change the directory to the specified folder, and pass in dummy args to command line parser
        // this will initialize the paths throughout E+ to the defaults
        DisplayString("EnergyPlus Library: Changing directory to: " + filepath);
#ifdef _WIN32
        int status = _chdir(filepath.c_str());
#else
        int status = chdir(filepath.c_str());
#endif
        if (status == 0) {
            DisplayString("Directory change successful.");
        } else {
            DisplayString("Couldn't change directory; aborting EnergyPlus");
            return EXIT_FAILURE;
        }
        DataStringGlobals::ProgramPath = filepath + DataStringGlobals::pathChar;
        int dummy_argc = 1;
        const char *dummy_argv[1] = {"energyplus"};
        CommandLineInterface::ProcessArgs(state, dummy_argc, dummy_argv);
    }

    int errStatus = initErrorFile();
    if (errStatus) {
        return errStatus;
    }

    DataSystemVariables::TestAllPaths = true;

    DisplayString("EnergyPlus Starting");
    DisplayString(DataStringGlobals::VerString);

    try {
        EnergyPlus::inputProcessor = InputProcessor::factory();
        EnergyPlus::inputProcessor->processInput();
        if (DataGlobals::outputEpJSONConversionOnly) {
            DisplayString("Converted input file format. Exiting.");
            return EndEnergyPlus();
        }
        ResultsFramework::OutputSchema->setupOutputOptions();
    } catch (const FatalError &e) {
        return AbortEnergyPlus(state);
    } catch (const std::exception &e) {
        ShowSevereError(e.what());
        return AbortEnergyPlus(state);
    }
    return 0;
}

int initializeAsLibrary(EnergyPlus::EnergyPlusData &state) {
    using namespace EnergyPlus;

    // Disable C++ i/o synching with C methods for speed
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr); // Untie cin and cout: Could cause odd behavior for interactive prompts

// Enable floating point exceptions
#ifndef NDEBUG
#ifdef __unix__
    feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif
#endif

#ifdef _MSC_VER
    #ifndef _DEBUG
    // If _MSC_VER and not debug then prevent dialogs on error
    SetErrorMode(SEM_NOGPFAULTERRORBOX);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
#endif

    DataSystemVariables::Time_Start = DataTimings::epElapsedTime();
#ifdef EP_Detailed_Timings
    epStartTime("EntireRun=");
#endif

    DataStringGlobals::CurrentDateTime = CreateCurrentDateTimeString();

    ResultsFramework::OutputSchema->SimulationInformation.setProgramVersion(DataStringGlobals::VerString);
    ResultsFramework::OutputSchema->SimulationInformation.setStartDateTimeStamp(DataStringGlobals::CurrentDateTime.substr(5));

    DataStringGlobals::VerString += "," + DataStringGlobals::CurrentDateTime;

    DataSystemVariables::processEnvironmentVariables(state.dataGlobals);

    int errStatus = initErrorFile();
    if (errStatus) {
        return errStatus;
    }

    DataSystemVariables::TestAllPaths = true;

    DisplayString("EnergyPlus Starting");
    DisplayString(DataStringGlobals::VerString);

    try {
        EnergyPlus::inputProcessor = InputProcessor::factory();
        EnergyPlus::inputProcessor->processInput();
        ResultsFramework::OutputSchema->setupOutputOptions();
    } catch (const FatalError &e) {
        return AbortEnergyPlus(state);
    } catch (const std::exception &e) {
        ShowSevereError(e.what());
        return AbortEnergyPlus(state);
    }
    return 0;
}

int wrapUpEnergyPlus(EnergyPlus::EnergyPlusData &state) {
    using namespace EnergyPlus;

    try {
        ShowMessage("Simulation Error Summary *************");

        GenOutputVariablesAuditReport();

        Psychrometrics::ShowPsychrometricSummary();

        EnergyPlus::inputProcessor->reportOrphanRecordObjects();
        FluidProperties::ReportOrphanFluids();
        ScheduleManager::ReportOrphanSchedules();

        if (DataGlobals::runReadVars) {
            int status = CommandLineInterface::runReadVarsESO();
            if (status) {
                return status;
            }
        }
    } catch (const FatalError &e) {
        return AbortEnergyPlus(state);
    } catch (const std::exception &e) {
        ShowSevereError(e.what());
        return AbortEnergyPlus(state);
    }

    return EndEnergyPlus();
}

int RunEnergyPlus(EnergyPlus::EnergyPlusData &state, std::string const & filepath)
{


    // PROGRAM INFORMATION:
    //       AUTHOR         Linda K. Lawrie, et al
    //       DATE WRITTEN   January 1997.....
    //       MODIFIED       na
    //       RE-ENGINEERED  na

    // PURPOSE OF THIS PROGRAM:
    // This program implements the calls for EnergyPlus (originally configured
    // as the merger of BLAST/IBLAST and DOE-2 energy analysis programs).

    // METHODOLOGY EMPLOYED:
    // The method used in EnergyPlus is to simplify the main program as much
    // as possible and contain all "simulation" code in other modules and files.

    int status = initializeEnergyPlus(state, filepath);
    if (status) return status;
    try {
        EnergyPlus::SimulationManager::ManageSimulation(state,EnergyPlus::OutputFiles::getSingleton());
    } catch (const EnergyPlus::FatalError &e) {
        return EnergyPlus::AbortEnergyPlus(state);
    } catch (const std::exception &e) {
        EnergyPlus::ShowSevereError(e.what());
        return EnergyPlus::AbortEnergyPlus(state);
    }
    return wrapUpEnergyPlus(state);
}

int runEnergyPlusAsLibrary(EnergyPlus::EnergyPlusData &state, int argc, const char *argv[])
{
    // PROGRAM INFORMATION:
    //       AUTHOR         Linda K. Lawrie, et al
    //       DATE WRITTEN   January 1997.....
    //       MODIFIED       na
    //       RE-ENGINEERED  na

    // PURPOSE OF THIS PROGRAM:
    // This program implements the calls for EnergyPlus (originally configured
    // as the merger of BLAST/IBLAST and DOE-2 energy analysis programs).

    // METHODOLOGY EMPLOYED:
    // The method used in EnergyPlus is to simplify the main program as much
    // as possible and contain all "simulation" code in other modules and files.

    EnergyPlus::DataGlobals::eplusRunningViaAPI = true;

    // clean out any stdin, stderr, stdout flags from a prior call
    if (!std::cin.good()) std::cin.clear();
    if (!std::cerr.good()) std::cerr.clear();
    if (!std::cout.good()) std::cout.clear();

    EnergyPlus::CommandLineInterface::ProcessArgs(state, argc, argv );

    int status = initializeAsLibrary(state);
    if (status) return status;
    try {
        EnergyPlus::SimulationManager::ManageSimulation(state, EnergyPlus::OutputFiles::getSingleton());
    } catch (const EnergyPlus::FatalError &e) {
        return EnergyPlus::AbortEnergyPlus(state);
    } catch (const std::exception &e) {
        EnergyPlus::ShowSevereError(e.what());
        return EnergyPlus::AbortEnergyPlus(state);
    }
    return wrapUpEnergyPlus(state);
}

void StoreProgressCallback(void (*f)(int const))
{
    using namespace EnergyPlus::DataGlobals;
    fProgressPtr = f;
}
void StoreMessageCallback(void (*f)(std::string const &))
{
    using namespace EnergyPlus::DataGlobals;
    fMessagePtr = f;
}

std::string CreateCurrentDateTimeString()
{

    // SUBROUTINE INFORMATION:
    //       AUTHOR         Linda Lawrie
    //       DATE WRITTEN   October 2010
    //       MODIFIED       na
    //       RE-ENGINEERED  na

    // PURPOSE OF THIS SUBROUTINE:
    // Be able to supply a current date/time string from intrinsic calls so
    // that one is always available.

    // SUBROUTINE PARAMETER DEFINITIONS:

    // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
    Array1D_int value(8);
    // value(1)   Current year
    // value(2)   Current month
    // value(3)   Current day
    // value(4)   Time difference with respect to UTC in minutes (0-59)
    // value(5)   Hour of the day (0-23)
    // value(6)   Minutes (0-59)
    // value(7)   Seconds (0-59)
    // value(8)   Milliseconds (0-999)
    std::string datestring; // supposedly returns blank when no date available.

    date_and_time(datestring, _, _, value);
    if (!datestring.empty()) {
        return EnergyPlus::format(" YMD={:4}.{:02}.{:02} {:02}:{:02}", value(1), value(2), value(3), value(5), value(6));
    } else {
        return " unknown date/time";
    }
}
