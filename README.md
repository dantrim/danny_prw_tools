# danny_prw_tools
Danny PRW tools, and so can you!

# Installation and Running
```bash
mkdir analysis_dir/; cd analysis_dir;
mkdir source/; cd source/;
lsetup "AnalysisBase,21.2,latest,here"
git clone git@github.com:dantrim/danny_prw_tools.git
cd ..
mkdir build; cd build;
cmake ../source
make -j
source x86*/setup.sh
run_mu -i <input-DAOD>
```
