// EDM
#include "AsgTools/ToolHandle.h"
#include "xAODCore/ShallowCopy.h"

// Base
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "AsgTools/AnaToolHandle.h"

// tools
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"
#include "PathResolver/PathResolver.h"

// std/stl
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <stdexcept>
using namespace std;

// ROOT
#include "TFile.h"
#include "TH1F.h"

void print_usage(string name)
{
    cout << name << endl;
    cout << endl;
    cout << " Options:" << endl;
    cout << "   -i|--input          input DAOD file" << endl;
    cout << "   -n|--nevents        number of events to process [default: all]" << endl;
    cout << "   -h|--help           print this help message" << endl;
    cout << " Example:" << endl;
    cout << "  " << name << " -i <input-file> -n 500" << endl;
}

std::vector<string> get_lumicalc_files()
{

    bool lumi_calc_ok = true;
    std::vector<string> lumi_calcs;
    // push them all back
    // 2015
    string lumi_calc_2015_loc = "GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root";
    string lumi_calc_2015 = PathResolverFindCalibFile(lumi_calc_2015_loc); // FindDataFile
    if(lumi_calc_2015=="") {
        cout << "XaodAnalysis::prw_ilumicalc_files    ERROR PathResolver unable to find "
            << "2015 ilumicalc file (=" << lumi_calc_2015_loc << ")" << endl;
        lumi_calc_ok = false;
    }
    lumi_calcs.push_back(lumi_calc_2015);
    
    // 2016
    string lumi_calc_2016_loc = "GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root";
    string lumi_calc_2016 = PathResolverFindCalibFile(lumi_calc_2016_loc);
    if(lumi_calc_2016=="") {
        cout << "XaodAnalysis::prw_ilumicalc_files    ERROR PathResolver unable to find "
            << "2016 ilumicalc file (=" << lumi_calc_2016_loc << ")" << endl;
        lumi_calc_ok = false;
    }
    lumi_calcs.push_back(lumi_calc_2016);

    // 2017
    string lumi_calc_2017_loc = "GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root"; // lumi-calc
    string lumi_calc_2017 = PathResolverFindCalibFile(lumi_calc_2017_loc); // FindDataFile
    if(lumi_calc_2017=="") {
        cout << "XaodAnalysis::prw_ilumicalc_files    ERROR PathResolver unable to find "
            << "2017 ilumicalc file (=" << lumi_calc_2017_loc << ")" << endl;
        lumi_calc_ok = false;
    }
    lumi_calcs.push_back(lumi_calc_2017);
    // 2018
    string lumi_calc_2018_loc = "GoodRunsLists/data18_13TeV/20180924/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-001.root";
    string lumi_calc_2018 = PathResolverFindCalibFile(lumi_calc_2018_loc);
    if(lumi_calc_2018=="") {
        cout << "XaodAnalysis::prw_ilumicalc_files    ERROR PathResolver unable to find "
            << "2018 lumicalc file (=" << lumi_calc_2018_loc << ")" << endl;
        lumi_calc_ok = false;
    }
    lumi_calcs.push_back(lumi_calc_2018);

    if(!lumi_calc_ok) exit(1);
    
    cout << "prw_ilumicalc_files    Loading " << lumi_calcs.size() << " lumicalc files" << endl;
    for(auto f : lumi_calcs) {
        cout << "prw_ilumicalc_files     > " << f << endl;
    }

    return lumi_calcs;
}

std::vector<string> get_prw_configs()
{

    std::vector<string> prw_out;

    bool file_ok = true;
    string actual_mu_17_loc = "GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.actualMu.OflLumi-13TeV-010.root";
    string actual_mu_17 = PathResolverFindCalibFile(actual_mu_17_loc);
    if(actual_mu_17 == "") {
        cout << "get_prw_configs    "
            << "Failed to open actualMu file for 2017 (=" << actual_mu_17_loc << ")" << endl;
        file_ok = false;
    }
    else {
        cout << "get_prw_configs    "
            << "Actual mu file for 2017: " << actual_mu_17 << endl;
        prw_out.push_back(actual_mu_17);
    }

    string actual_mu_18_loc = "GoodRunsLists/data18_13TeV/20180924/physics_25ns_Triggerno17e33prim.actualMu.OflLumi-13TeV-001.root";
    string actual_mu_18 = PathResolverFindCalibFile(actual_mu_18_loc);
    if(actual_mu_18 == "") {
        cout << "get_prw_configs    "
            << "Failed to open actualMu file for 2018 (=" << actual_mu_18_loc << ")" << endl;
        file_ok = false;
    }
    else {
        cout << "get_prw_configs    "
            << "Actual mu file for 2018: " << actual_mu_18 << endl;
        prw_out.push_back(actual_mu_18);
    }

    cout << "get_prw_configs    Loading " << prw_out.size() << " prw files " << endl;
    for(auto f : prw_out) {
        cout << "get_prw_configs    > " << f << endl;
    }

    return prw_out;
}


int main(int argc, char** argv)
{
    const char * algo_name = argv[0];
    string filename = "";
    int n_to_process = -1;
    int optin = 1;
    if(argc == 1) {
        print_usage(algo_name);
        return 1;
    }
    while(optin < argc) {
        string in = argv[optin];
        if(in == "-i" || in == "--input") filename = argv[++optin];
        else if(in == "-n" || in == "--nevents") n_to_process = atoi(argv[++optin]);
        else if(in == "-h" || in == "--help") { print_usage(algo_name); return 0; }
        else {
            cout << algo_name << " Unknown command line argument '" << in << "' provided, exiting" << endl;
            return 1;
        }
        optin++;
    } // while

    if(filename == "") {
        cout << algo_name << "    ERROR you must provide an input file" << endl;
        return 1;
    }

    RETURN_CHECK(algo_name, xAOD::Init());
    xAOD::TEvent event(xAOD::TEvent::kClassAccess);

    std::unique_ptr<TFile> ifile(TFile::Open(filename.c_str(), "READ"));
    if(!ifile.get() || ifile->IsZombie()) {
        throw std::logic_error("ERROR Could not open input file: " + filename);
    }

    TFile* out_rfile = new TFile("mu_plots.root", "RECREATE");
    TH1F* hmu = new TH1F("hmu", ";<mu> or mu;", 80, 0, 80);
    TH1F* hmu_actual = new TH1F("hmu_actual", ";<mu> or mu;", 80, 0, 80);

    auto prw_configs = get_prw_configs();
    auto lumi_calc_files = get_lumicalc_files();

    // initialize the tools
    asg::AnaToolHandle<CP::IPileupReweightingTool> m_prwTool("");
    stringstream toolname;
    toolname << "CP::PileupReweightingTool/PRWTool";
    m_prwTool.setTypeAndName(toolname.str());
    RETURN_CHECK( algo_name, m_prwTool.setProperty("ConfigFiles", prw_configs) );
    RETURN_CHECK( algo_name, m_prwTool.setProperty("LumiCalcFiles", lumi_calc_files) );
    RETURN_CHECK( algo_name, m_prwTool.setProperty("DataScaleFactor", 1./1.03) );
    RETURN_CHECK( algo_name, m_prwTool.setProperty("DataScaleFactorUP", 1./0.99) );
    RETURN_CHECK( algo_name, m_prwTool.setProperty("DataScaleFactorDOWN", 1./1.07) );
    RETURN_CHECK( algo_name, m_prwTool.setProperty("OutputLevel", MSG::VERBOSE) );
    RETURN_CHECK( algo_name, m_prwTool.retrieve() );

    // start looping
    RETURN_CHECK(algo_name, event.readFrom(ifile.get()));
    const unsigned long long n_entries = event.getEntries();
    cout << algo_name << "    Input file has " << n_entries << " events";
    if(n_to_process < 0) n_to_process = n_entries;
    cout << ", will read " << n_to_process << " events" << endl;

    int n_mu = 0;
    int n_el = 0;

    for(unsigned long long entry = 0; entry < (unsigned)n_to_process; ++entry) {

        bool ok = event.getEntry(entry) >= 0;
        if(!ok) throw std::logic_error("ERROR getEntry failed");

        if(entry % 500 == 0) {
            cout << algo_name << "   Processing entry " << entry << endl;
        }

        const xAOD::EventInfo* ei = 0;
        RETURN_CHECK(algo_name, event.retrieve(ei, "EventInfo"));

        float mu = m_prwTool->getCorrectedAverageInteractionsPerCrossing(*ei);
        float mu_actual = m_prwTool->getCorrectedActualInteractionsPerCrossing(*ei);
        hmu->Fill(mu);
        hmu_actual->Fill(mu_actual);
    } // entry


    cout << "--------------------------------------" << endl;
    cout << " done looping" << endl;
    hmu->SetLineColor(kRed);
    hmu_actual->SetLineColor(kBlue);
    out_rfile->cd();
    hmu->Write();
    hmu_actual->Write();
    out_rfile->Write();
    
    // clean up memory.. meh

    return 0;
}
