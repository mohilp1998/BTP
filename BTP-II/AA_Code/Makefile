# build software version of testbench (to check the "desired behaviour")
AHIR_INCLUDE=$(AHIR_RELEASE)/include
AHIR_LIB=$(AHIR_RELEASE)/lib
VHDL_LIB=$(AHIR_RELEASE)/vhdl
VHDL_VHPI_LIB=$(AHIR_RELEASE)/CtestBench/vhdl
FUNCTIONLIB=$(AHIR_RELEASE)/functionLibrary/
SRC=./src
all: HW  aa2c
TOVC:aalink aa2vc 
VC2VHDL: vc2vhdl  vhdlsim
AA2VHDLSIM: aa2vc  vc2vhdl  vhdlsim
TOVHDL:TOVC vc2vhdl

TOPMODULES=-T top_module



# five steps from C to vhdl simulator.
HW: aalink aa2vc  vc2vhdl  vhdlsim

AA2VHDL: aa2vc vc2vhdl vhdlsim 

# Aa to vC
aalink: $(SRC)/prog.aa 
	AaLinkExtMem $(SRC)/prog.aa | vcFormat > prog.linked.aa
	AaOpt -B prog.linked.aa | vcFormat > prog.linked.opt.aa

aa2c: prog.linked.opt.aa
	rm -rf aa2c
	mkdir aa2c
	Aa2C -P mem_ -o aa2c $(TOPMODULES) prog.linked.opt.aa 
	gcc -g -c -I $(AHIR_INCLUDE) -I aa2c/ -o aa2c/mem_aa_c_model.o aa2c/mem_aa_c_model.c
	gcc -g -c -I $(AHIR_INCLUDE) -I aa2c/ -D AA2C -o aa2c/testbench.o $(SRC)/testbench.c
	gcc -g -o testbench_aa2c aa2c/testbench.o aa2c/mem_aa_c_model.o -lBitVectors -lSockPipes -lPipeHandlerDebugPthreads -lpthread -L $(AHIR_LIB) 

indentc: aa2c/mem_aa_c_model.c
	indent aa2c/mem_aa_c_model.c
	indent aa2c/mem_aa_c_model.h
	indent aa2c/mem_aa_c_model_internal.h



aa2vc: prog.linked.opt.aa
	Aa2VC -O -C prog.linked.opt.aa | vcFormat > prog.vc

# vC to VHDL
vc2vhdl: prog.vc
	vc2vhdl -O -S 4 -I 2 -v -a -C -e ahir_system -w -s ghdl $(TOPMODULES) -f prog.vc 
	vhdlFormat < ahir_system_global_package.unformatted_vhdl > ahir_system_global_package.vhdl
	vhdlFormat < ahir_system.unformatted_vhdl > ahir_system.vhdl
	vhdlFormat < ahir_system_test_bench.unformatted_vhdl > ahir_system_test_bench.vhdl

# build testbench and ghdl executable
# note the use of libVhpi in building the testbench.
vhdlsim: ahir_system.vhdl ahir_system_test_bench.vhdl $(SRC)/testbench.c vhdlCStubs.h vhdlCStubs.c
	gcc -c vhdlCStubs.c  -I$(SRC) -I./ -I$(AHIR_INCLUDE)
	gcc -c $(SRC)/testbench.c -I$(AHIR_INCLUDE) -I$(SRC) -I./
	gcc -o testbench_hw testbench.o vhdlCStubs.o  -L$(AHIR_LIB) -lSocketLibPipeHandler -lpthread
	ghdl --clean
	ghdl --remove
	ghdl -i --work=GhdlLink  $(VHDL_LIB)/GhdlLink.vhdl
	ghdl -i --work=aHiR_ieee_proposed  $(VHDL_LIB)/aHiR_ieee_proposed.vhdl
	ghdl -i --work=ahir  $(VHDL_LIB)/ahir.vhdl
	ghdl -i --work=work ahir_system_global_package.vhdl
	ghdl -i --work=work ahir_system.vhdl
	ghdl -i --work=work ahir_system_test_bench.vhdl
	ghdl -m --work=work -Wl,-L$(AHIR_LIB) -Wl,-lVhpi ahir_system_test_bench 

clean:
	rm -rf *.o* *.cf *.*vhdl vhdlCStubs.* *.vcd in_data* out_data* testbench_sw testbench_hw ahir_system_test_bench vhpi.log *.aa *.vc *.lso xst *.ngc *_xmsgs *.xrpt pipeHandler.log *.srp *.ghw *.dot aa2c/ testbench_aa2c .gsi.aa *eps

PHONY: all clean	
