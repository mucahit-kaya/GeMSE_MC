#include "GeMSE_Analysis.hh"

#include "globals.hh"

#include "TH1.h"
#include "TH1D.h"


GeMSE_Analysis::GeMSE_Analysis() {
  static bool once = [](){
  TH1::AddDirectory(kFALSE);
  return true;
  }();
  (void)once;

  fhTotEdep = new TH1D("hTotEdep", ";Energy (keV);Counts", 30000, 0., 3000.);
}

GeMSE_Analysis::~GeMSE_Analysis() {
  delete fhTotEdep;
  fenergy.clear();
  fwidth_sig.clear();
  fwidth_bkg.clear();
  fNuDatBR.clear();
  fG4BR.clear();
  fefficiency.clear();
  fefficiency_err.clear();
  feffBR.clear();
}

// Get efficiency for single line
void GeMSE_Analysis::CalcEfficiencies() {
  fnlines = fenergy.size();

  // loop over all gamma lines
  for (int i = 0; i < fnlines; ++i) {
    double energy = fenergy[i];
    double width_sig = fwidth_sig[i];
    double width_bck = fwidth_bkg[i];
    double BR_NuDat = fNuDatBR[i];
    double BR_G4 = fG4BR[i];

    // counts in peak
    double bin_min_peak = fhTotEdep->FindBin(energy - width_sig / 2.);
    double bin_max_peak = fhTotEdep->FindBin(energy + width_sig / 2.);
    double sum_peak = fhTotEdep->Integral(bin_min_peak, bin_max_peak);
    double sum_peak_err = sqrt(sum_peak);
    int nBins_peak = bin_max_peak - bin_min_peak + 1;

    // calculate counts in left bck region
    double bin_min_bckleft =
        fhTotEdep->FindBin(energy - width_bck / 2. - width_sig / 2.);
    double bin_max_bckleft = fhTotEdep->FindBin(energy - width_bck / 2.) - 1;
    double sum_bckleft = fhTotEdep->Integral(bin_min_bckleft, bin_max_bckleft);
    double sum_bckleft_err = sqrt(sum_bckleft);
    int nBins_bckleft = bin_max_bckleft - bin_min_bckleft + 1;

    // calculate counts in right bck region
    double bin_min_bckright = fhTotEdep->FindBin(energy + width_sig / 2.) + 1;
    double bin_max_bckright =
        fhTotEdep->FindBin(energy + width_sig / 2. + width_bck / 2.);
    double sum_bckright =
        fhTotEdep->Integral(bin_min_bckright, bin_max_bckright);
    double sum_bckright_err = sqrt(sum_bckright);
    int nBins_bckright = bin_max_bckright - bin_min_bckright + 1;

    // substract bck counts from peak
    double f_bckleft = nBins_peak / 2. / nBins_bckleft;
    double f_bckright = nBins_peak / 2. / nBins_bckright;

    double counts =
        sum_peak - f_bckleft * sum_bckleft - f_bckright * sum_bckright;
    double counts_err = sqrt(sum_peak_err * sum_peak_err +
                             f_bckleft * sum_bckleft_err * sum_bckleft_err +
                             f_bckright * sum_bckright_err * sum_bckright_err);
    double efficiency = counts / fNEvents / BR_G4;
    if (efficiency < 0)
      G4cout << "\n\n##### WARNING " << G4endl
             << "Efficiency is smaller than zero for " << energy
             << " keV! --> " << efficiency << G4endl
             << "  sum_peak = " << sum_peak << G4endl
             << "  f_bckleft*sum_bckleft = " << f_bckleft * sum_bckleft << G4endl
             << "  f_bckright*sum_bckright = " << f_bckright * sum_bckright << G4endl
             << "  counts =  " << counts << G4endl;

    double efficiency_err = counts_err / fNEvents / BR_G4;

    fefficiency.push_back(efficiency);
    fefficiency_err.push_back(efficiency_err);

    feffBR.push_back(efficiency * BR_NuDat);
  }
}

// Clear all data
void GeMSE_Analysis::Clear() {
  fenergy.clear();
  fwidth_sig.clear();
  fwidth_bkg.clear();
  fNuDatBR.clear();
  fG4BR.clear();
  fefficiency.clear();
  fefficiency_err.clear();
  feffBR.clear();
}
