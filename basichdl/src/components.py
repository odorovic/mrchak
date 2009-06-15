'''
Created on Apr 5, 2009

@author: odor
'''

import hdl 
from myhdl import instance, delay, always, Signal, intbv, always_comb, instances
from myhdl import Tristate


def PROBE(sig, **kwargs):
    '''Komponenta koja detektuje uzlaznu ivicu signala na koji je vezana.'''
    @always(sig.posedge)
    def detect():
        print "Detektovan posedge: %s" % sig._name
        evt = hdl.PosEdgeEvt(sig)
        hdl.process_event(evt)
    return detect

def CLOCK(clk, period=20, **kwargs):
    highTime = int(period/5)
    lowTime = period - highTime
    @instance
    def driveClk():
        while True:
            yield delay(lowTime)
            clk.next = 1
            yield delay(highTime)
            clk.next = 0
    return driveClk

def MUX2(in0, in1, S, out, **kwargs):
    '''Dvoulazni multiplekser.'''
    
    @always_comb
    def eval_mux():
        if S == 0:
            out.next = in0
        elif S == 1:
            out.next = in1
        else:
            assert False, "S signal multipleksera nije ni 1 ni 0. Weird."
    return eval_mux

def MUX4(in0,in1,in2,in3,S0,S1,out, **kwargs):
    @always_comb
    def eval_mux():
        if S0 == 0 and S1 == 0:     #0
            out.next = in0
        elif S0 == 1 and S1 == 0:   #1
            out.next = in1
        elif S0 == 0 and S1 == 1:   #2
            out.next = in2
        elif S0 == 1 and S1 == 1:   #3
            out.next = in3
        else:
            assert False, "S signal multipleksera nije ni 1 ni 0. Weird."
    return eval_mux

def ROM(input, output, name=None):
    '''Asinhrona rom memorija.'''
    assert name != None
    memory = {'name' : name,
              'addr_width': len(input), 
              'data_width': len(output),
              'access': 'r'}
    hdl.request_memory(memory)
    @always_comb
    def eval_rom():
        newstate = hdl.read_memory(memory['name'], input.val)
        output.next = newstate
    return eval_rom
        
def AND(in1, in2, out, **kwargs):
    assert len(in1) == len(in2) == len(out)    
    @always_comb
    def eval_and():
        out.next = in1 & in2
    return eval_and

def ANDPN(in1, in2, out, **kwargs):
    '''AND kolo sa negiranim drugim ulazom'''
    assert len(in1) == len(in2) == len(out)    
    @always_comb
    def eval_and():
        out.next = in1 & ~in2
    return eval_and

def ANDNP(in1, in2, out, **kwargs):
    '''AND kolo sa negiranim prvim ulazom'''
    assert len(in1) == len(in2) == len(out)    
    @always_comb
    def eval_and():
        out.next = ~in1 & in2
    return eval_and

def OR(in1, in2, out, **kwargs):
    assert len(in1) == len(in2) == len(out)
    @always_comb
    def eval_or():
        out.next = in1 | in2
    return eval_or

def OR4(in1, in2, in3, in4, out, **kwargs):
    assert len(in1) == len(in2) == len(out)
    @always_comb
    def eval_or():
        out.next = in1 | in2 | in3 | in4
    return eval_or
    
def NAND(in1, in2, out, **kwargs):
    assert len(in1) == len(in2) == len(out)    
    @always_comb
    def eval_nand():
            out.next = ~(in1 & in2)
    return eval_nand
    
def NOR(in1, in2, out, **kwargs):
    assert len(in1) == len(in2) == len(out)
    outlen = len(out)
    @always_comb
    def eval_nor():
        out.next = (~(in1 | in2))[outlen:0]
    return eval_nor

def NORBUS(input, output, **kwargs):
    assert len(output) == 1
    @always_comb
    def eval_nor():
        if input == 0:
            output.next = 1
        else:
            output.next = 0
    return eval_nor
       
def NOT(input, out, **kwargs):
    assert len(input) == len(out)
    outlen = len(out)
    @always_comb
    def eval_not():
        if input.val != None:
            out.next = (~input)[outlen:0]
        else:
            out.next = 0
    return eval_not

def ADD(in1, in2, out, **kwargs):
    
    @always_comb
    def eval_add():
        out.next = in1 + in2
    return eval_add

def ASYNCRSFF(S,R,Q,NOTQ, **kwargs):
    
    def eval(s, r):
        print 'posle', s, r
        if s == 1 and r == 0:
            print 'set'
            Q.next = 1
            NOTQ.next = 0
        elif s == 0 and r == 1:
            print 'reset'
            Q.next = 0
            NOTQ.next = 1
        elif s == 0 and r == 0:
            print 'hold'
            Q.next = Q
            NOTQ.next = ~Q
        else:
            #Ovde imamo nedefinisano stanje. Posto myhdl signali ne podrzavaju.
            #radimo ono sto pravi RSFF radi: naizmenicno smenjujemo ulaze.
            print 'ndef'
            Q.next = NOTQ
            NOTQ.next = Q
    
    @instance
    def gen_ff():
        yield delay(2)
        eval(S.val, R.val)
        while True:
            yield S, R
            yield delay(1)
            s = S.val
            r = R.val
            yield delay(1)
            eval(s,r)
        
    return gen_ff

def REG(ld, input, clk, out, name=None):
    '''Sinhroni registar.
    
    Stanje registra je jednoznacno odredjeno stanjem njegovog izlaznog signala.
    Ipak
    '''
    assert name != None
    assert len(ld) == 1
    assert len(clk) == 1
    assert len(input) == len(out)
    memory = {'buffer' : None, 'output': out, 'name':name}
    hdl.register_reg(memory)    #prijavljujemo komponentu, za kasnije
    reg_delay = 5
    
    @instance
    def eval_reg():
        '''Generator vrsi sinhroni upis u registar.
        
        Na uzlaznu ivicu takta pamtimo stanje ulaza u bafer, cekamo reg_delay
        te vrednost iz bafera izbacujemo na izlaz. 
        
        Provera da li je bafe != None je prouzrokovna cinjenicom da u toku
        kasnjenja registra, neko je mogao da unese novu vrednost asinhrono
        '''
        while True:
            yield clk.posedge
            if(ld.val == 1):
                if name == 'MBR':
                    print 'loudujemo MBR'
                assert input.val != None, str(name) + " " + str(input)
                memory['buffer'] = input.val
                yield delay(reg_delay)
            if memory['buffer'] != None:
                out.next = memory['buffer']
                memory['buffer'] = None
    return eval_reg

def INCREG(ld, inc, input, clk, output, name=None):
    '''Sinhroni registar.
    
    Stanje registra je jednoznacno odredjeno stanjem njegovog izlaznog signala.
    Ipak
    '''
    assert name != None
    assert len(ld) == 1
    assert len(clk) == 1
    assert len(inc) == 1
    assert len(input) == len(output)
    memory = {'buffer' : None, 'output': output, 'name':name}
    hdl.register_reg(memory)    #prijavljujemo komponentu, za kasnije
    reg_delay = 5
    
    @instance
    def eval_increg():
        '''Generator vrsi sinhroni upis u registar.
        
        Na uzlaznu ivicu takta pamtimo stanje ulaza u bafer, cekamo reg_delay
        te vrednost iz bafera izbacujemo na izlaz. 
        
        Provera da li je bafe != None je prouzrokovna cinjenicom da u toku
        kasnjenja registra, neko je mogao da unese novu vrednost asinhrono
        '''
        while True:
            yield clk.posedge
            if ld.val == 1:
                assert input.val != None
                memory['buffer'] = input.val
                yield delay(reg_delay)
            elif inc.val == 1:
                memory['buffer'] = output.val + 1
                yield delay(reg_delay)
            if memory['buffer'] != None:
                output.next = memory['buffer']
                memory['buffer'] = None
    return eval_increg

def ALUREG(Yin, YselM1, M1, M2, Ysignex, clk, output, name=None):
    '''Prihvatni registar ALU jedinice.
    
    Obichan registar sa okolnom kombinacionom mrezom koja prosledjuje podatke
    sa dve magistrale i opciono prosiruje mladji bajt reci znakom.
    '''
    assert len(M1) == 16
    assert len(M2) == 16
    assert len(output) == 16
    memory = {'buffer' : None, 'output': output, 'name':name}
    hdl.register_reg(memory)    #prijavljujemo komponentu, za kasnije
    reg_delay = 5
    
    def eval_input():
        if YselM1 == 1:
            if Ysignex == 0:
                return M1.val
            else:
                tmp = intbv(0)[16:0]
                tmp[8:0] = M1[8:0]
                if tmp[7] == True:
                    ext = 255
                else:
                    ext = 0
                tmp[16:8] = ext
                return tmp
        else:
            return M2.val       
    
    @instance
    def eval_reg():
        '''Generator vrsi sinhroni upis u registar.
        
        Na uzlaznu ivicu takta pamtimo stanje ulaza u bafer, cekamo reg_delay
        te vrednost iz bafera izbacujemo na izlaz. 
        
        Provera da li je bafe != None je prouzrokovna cinjenicom da u toku
        kasnjenja registra, neko je mogao da unese novu vrednost asinhrono
        '''
        while True:
            yield clk.posedge
            if(Yin.val == 1):
                assert eval_input() != None
                memory['buffer'] = eval_input()
                yield delay(reg_delay)
            if memory['buffer'] != None:
                output.next = memory['buffer']
                memory['buffer'] = None
    return eval_reg


def TEMPREG(TEMPinHIGH, TEMPinLOW, input, TEMPswap, clk, output, name=None):
    '''Temp registar, sa baferom izmedju 16 i 8 bitih reci.
    
    Obican registar sa kombinacionom mrezom koja manipulishe upisom
    pojedinacnih bajtova.
    
    Ako je na uzlaznu ivicu takta aktivan TEMPinHIGH, nizi bajt sa ulaza se
    upisuje na visi bajt registra bez izmene nizeg.
    Ako je aktivan TEMPinLOW, upisuje se nizi bajt. Ako su oba signala aktivna,
    upisuje se cela rec.
    
    Ako je TEMPswap aktivan, bajtovi na izlazu menjaju mesta.
    '''
    assert len(input) == 16
    assert len(output) == 16
    memory = {'buffer' : None, 'output': output, 'name':name}
    hdl.register_reg(memory)    #prijavljujemo komponentu, za kasnije
    reg_delay = 5
    
    def eval_input():
        tmp = intbv(0)[16:0]
        if TEMPinHIGH == 1 and TEMPinLOW == 1:
            return input
        elif TEMPinHIGH == 1:
            tmp[16:8] = input[8:0]
            tmp[8:0] = output[8:0]
            print "TEMP: ", tmp
            return tmp
        elif TEMPinLOW == 1:
            tmp[16:8] = output[16:8]
            tmp[8:0] = input[8:0]
            print "TEMP: ", tmp
            return tmp
        else:
            print TEMPinHIGH.val, TEMPinLOW.val
            assert False, 'Temp reg error'
            
    def eval_output(value):
        tmp = intbv(0)[16:0]
        if TEMPswap == 0:
            return value
        else:
            tmp[16:8] = value[8:0]
            tmp[8:0] = value[16:8]
            return tmp
    
    @instance
    def eval_reg():
        '''Generator vrsi sinhroni upis u registar.
        
        Na uzlaznu ivicu takta pamtimo stanje ulaza u bafer, cekamo reg_delay
        te vrednost iz bafera izbacujemo na izlaz. 
        
        Provera da li je bafe != None je prouzrokovna cinjenicom da u toku
        kasnjenja registra, neko je mogao da unese novu vrednost asinhrono
        '''
        while True:
            rez = None
            yield clk.posedge
            print "tempreg.posedge"
            if TEMPinHIGH == 1 or TEMPinLOW == 1:
                print 'ucitavamo temp'
                memory['buffer'] = eval_input()
                rez = memory['buffer']
                yield delay(reg_delay)
                pass
            if rez != None:
                output.next = rez
                memory['buffer'] = None
    return eval_reg

def EXTRACT8_01(input, bit0, bit1, **kwargs):
    
    @always(input)
    def eval_comb():
        bit0.next = input[0]
        bit1.next = input[1]
    return eval_comb

def TRANS2_1(in1, in2, output, **kwargs):
    '''in1 - nizi bajt, in2 visi bajt'''
    assert len(in1) + len(in2) == len(output)
    s1,s2,s3 = len(in1), len(in1), len(output)
    
    @always(in1, in2)
    def eval_trans():
        tmp = intbv(0)[s3:0]
        tmp[s1:0] = in1.val
        tmp[s1+s2:s1] = in2.val
        output.next = tmp
    return eval_trans

def TRANS3_1(in1,in2,in3, out, **kwargs):
    '''Konkatinator 3 '''
    assert len(out) == 3
    assert len(in1)+len(in2)+len(in3) == len(out)
    
    @always_comb
    def eval_trans():
        i1 = in1.val
        i2 = in2.val
        i3 = in3.val
        outval = intbv(0)[3:0]
        outval[0] = i1
        outval[1] = i2
        outval[2] = i3
        out.next = outval
    return eval_trans

def TRANS4_1(in1,in2,in3, in4, out, **kwargs):
    '''Konkatinator 3 '''
    assert len(out) == 4
    assert len(in1)+len(in2)+len(in3)+len(in4) == len(out)
    
    @always_comb
    def eval_trans():
        i1 = in1.val
        i2 = in2.val
        i3 = in3.val
        i4 = in4.val
        outval = intbv(0)[4:0]
        outval[0] = i1
        outval[1] = i2
        outval[2] = i3
        outval[3] = i4
        out.next = outval
    return eval_trans

def TRANS1_2(input, out1,out2, **kwargs):
    '''Spliter. deli signal na 3 dela, srazmerno izlaznim signalima.
    
    Nizi izlazni pinovi primaju nize delove reci (vidi simbol)
    '''
    assert len(out1)+len(out2) == len(input)
    s0,s1, s2= 0,len(out1),len(out1)+len(out2)
    @always_comb
    def eval_trans():
        if input.val != None:
            out1.next = input.val[s1:s0]
            out2.next = input.val[s2:s1]
        else:
            out1.next = 0
            out2.next = 0
    return eval_trans

def TRANS1_3(input, out1,out2,out3, **kwargs):
    '''Spliter. deli signal na 3 dela, srazmerno izlaznim signalima.
    
    Nizi izlazni pinovi primaju nize delove reci (vidi simbol)
    '''
    assert len(out1)+len(out2)+len(out3) == len(input)
    s0,s1, s2,s3 = 0,len(out1),len(out1)+len(out2),len(out1)+len(out2)+len(out3) 
    @always_comb
    def eval_trans():
        out1.next = input[s1:s0]
        out2.next = input[s2:s1]
        out3.next = input[s3:s2]
    return eval_trans

def TRANS1_4(input, out1,out2,out3, out4, **kwargs):
    '''Spliter. deli signal na 3 dela, srazmerno izlaznim signalima.
    
    Nizi izlazni pinovi primaju nize delove reci (vidi simbol)
    '''
    lens = len(out1),len(out2),len(out3),len(out4),len(input)
    assert len(out1)+len(out2)+len(out3) + len(out4) == len(input), str(lens)
    s0 = 0
    s1 = len(out1)
    s2 = s1 +len(out2)
    s3 = s2+len(out3)
    s4 = s3+len(out4) 
    @always_comb
    def eval_trans():
        out1.next = input[s1:s0]
        out2.next = input[s2:s1]
        out3.next = input[s3:s2]
        out4.next = input[s4:s3]
    return eval_trans

def CTRLSIGNALS(ilop,iladr,I1,I2,I3,STORE,MEMINDIR,INTERRUPT, PSW, CW,
                branch,caseadr,caseadr2,caseop, **kwargs):
    '''Generisanje signala kontrolne jedinice. Za sada myhdl implementacija.'''
    assert len(PSW) == 16
    assert len(CW) == 8
    @always_comb
    def eval_sigs():
        '''Prost behiveioralni model'''
        Z = PSW.val[2]
        branch.next = 0
        caseadr.next = 0
        caseadr2.next = 0
        caseop.next = 0
        if CW[7]:   #ako uopste imamo neke skokove
            if CW[6]:   #uslovni skokovi
                jmp_code = CW[3:0]
                if jmp_code == 0 and ilop:
                    branch.next = 1
                elif jmp_code == 1 and iladr:
                    branch.next = 1
                elif jmp_code == 2 and not I1:
                    branch.next = 1
                elif jmp_code == 3 and not I2:
                    branch.next = 1
                elif jmp_code == 4 and not I3:
                    branch.next = 1
                elif jmp_code == 5 and STORE:
                    branch.next = 1
                elif jmp_code == 6 and MEMDIR:
                    branch.next = 1
                elif jmp_code == 7 and INTERRUPT:
                    branch.next = 1
                elif jmp_code == 8 and Z:
                    branch.next = 1
            else:   #case / bezuslovni skokovi
                jmp_code = CW[2:0]
                if jmp_code == 0:
                    branch.next = 1
                elif jmp_code == 1:
                    caseadr.next = 1
                elif jmp_code == 2:
                    caseadr2.next = 1
                elif jmp_code == 3:
                    caseop.next = 1
    return eval_sigs
                
def CD16(in0, in1, in2, in3, in4, in5, in6, in7, in8, in9,
        in10, in11, in12, in13, in14, in15, out0, out1, out2, out3, 
        **kwargs):
    '''16-bitni prioritetni koder. Mani ulaz => veci prioritet'''
    
    @always_comb
    def eval_dc():
        if in0:
            out0.next, out1.next, out2.next, out3.next = intbv(0)[4:0]
        elif in1:
            out0.next, out1.next, out2.next, out3.next = intbv(1)[4:0]
        elif in2:
            out0.next, out1.next, out2.next, out3.next = intbv(2)[4:0]
        elif in3:
            out0.next, out1.next, out2.next, out3.next = intbv(3)[4:0]
        elif in4:
            out0.next, out1.next, out2.next, out3.next = intbv(4)[4:0]
        elif in5:
            out0.next, out1.next, out2.next, out3.next = intbv(5)[4:0]
        elif in6:
            out0.next, out1.next, out2.next, out3.next = intbv(6)[4:0]
        elif in7:
            out0.next, out1.next, out2.next, out3.next = intbv(7)[4:0]
        elif in8:
            out0.next, out1.next, out2.next, out3.next = intbv(8)[4:0]
        elif in9:
            out0.next, out1.next, out2.next, out3.next = intbv(9)[4:0]
        elif in10:
            out0.next, out1.next, out2.next, out3.next = intbv(10)[4:0]
        elif in11:
            out0.next, out1.next, out2.next, out3.next = intbv(11)[4:0]
        elif in12:
            out0.next, out1.next, out2.next, out3.next = intbv(12)[4:0]
        elif in13:
            out0.next, out1.next, out2.next, out3.next = intbv(13)[4:0]
        elif in14:
            out0.next, out1.next, out2.next, out3.next = intbv(14)[4:0]
        elif in15:
            out0.next, out1.next, out2.next, out3.next = intbv(15)[4:0]
    return eval_dc
            
def CD8(in0, in1, in2, in3, in4, in5, in6, in7, E, out0, out1, out2, **kwargs):
    '''16-bitni prioritetni koder. Mani ulaz => veci prioritet'''
    
    @always_comb
    def eval_dc():
        if in0:
            out0.next, out1.next, out2.next = intbv(0)[3:0]
        elif in1:
            out0.next, out1.next, out2.next = intbv(1)[3:0]
        elif in2:
            out0.next, out1.next, out2.next = intbv(2)[3:0]
        elif in3:
            out0.next, out1.next, out2.next = intbv(3)[3:0]
        elif in4:
            out0.next, out1.next, out2.next = intbv(4)[3:0]
        elif in5:
            out0.next, out1.next, out2.next = intbv(5)[3:0]
        elif in6:
            out0.next, out1.next, out2.next = intbv(6)[3:0]
        elif in7:
            out0.next, out1.next, out2.next = intbv(7)[3:0]
        else:
            out0.next, out1.next, out2.next = intbv(0)[3:0]
    return eval_dc

def DEC4(in1, in2, E, out0, out1, out2, out3, **kwargs):
    
    @always(in1,in2,E)
    def eval_dec():
        for sig in out0, out1, out2, out3:
            sig.next = 0
        if E == 1:
            if in1 == 0 and in2 == 0:
                out0.next = 1
            if in1 == 1 and in2 == 0:
                out1.next = 1
            if in1 == 0 and in2 == 1:
                out2.next = 1
            if in1 == 1 and in2 == 1:
                out3.next = 1
    return eval_dec        
    
def DEC8(in1, in2, in3, E, out0, out1, 
         out2, out3, out4, out5, out6, out7, **kwargs):
    
    @always(in1,in2,in3,E)
    def eval_dec():
        for sig in out0, out1, out2, out3, out4, out5, out6, out7:
            sig.next = 0
        if E == 1:
            if in1 == 0 and in2 == 0 and in3 == 0:
                out0.next = 1
            if in1 == 1 and in2 == 0 and in3 == 0:
                out1.next = 1
            if in1 == 0 and in2 == 1 and in3 == 0:
                out2.next = 1
            if in1 == 1 and in2 == 1 and in3 == 0:
                out3.next = 1
            if in1 == 0 and in2 == 0 and in3 == 1:
                out4.next = 1
            if in1 == 1 and in2 == 0 and in3 == 1:
                out5.next = 1
            if in1 == 0 and in2 == 1 and in3 == 1:
                out6.next = 1
            if in1 == 1 and in2 == 1 and in3 == 1:
                out7.next = 1
    return eval_dec
    
def RAM(clk, ABUS, DBUS, NOTRDBUS, NOTWRBUS, NOTBUSYBUS, name=None):
      assert isinstance(ABUS, Tristate)
      assert isinstance(DBUS, Tristate)
      assert isinstance(NOTRDBUS, Tristate)
      assert isinstance(NOTWRBUS, Tristate)
      assert isinstance(NOTBUSYBUS, Tristate)
      
      assert name != None
      memory = {'name' : name,
              'addr_width': len(ABUS), 
              'data_width': len(DBUS),
              'access': 'rw'}
      hdl.request_memory(memory)
      
      datadriver = DBUS.driver()
      
      @always(clk.posedge, NOTRDBUS, NOTWRBUS)
      def eval_ram():
          print "EVAL_RAM"
          if NOTRDBUS.val == 0:
              assert NOTWRBUS.val == None, 'WR i RD magistrale aktivne istovremeno'
              newstate = hdl.read_memory(name, ABUS.val)
              datadriver.next = newstate
          elif NOTWRBUS.val == 0:
              assert NOTRDBUS.val == None, 'WR i RD magistrale aktivne istovremeno'
              newstate = hdl.write_memory(name, ABUS.val, DBUS.val)
          elif clk.val == 1:
              datadriver.next = None
              
      return eval_ram
  
def ARBITRATOR(busreq1, busreq2, busreq3, busreq4, 
               busgr1, busgr2, busgr3, busgr4, **kwargs):
    '''4-ulazni paralelni arbitrator magistrale.
    
    Za sada vrlo falicna (nikakva) implementacija. Zahtevi poliniji 1 se uvek odmah
    prihvataju, ostali zahtevi se ignorishu. 
    '''
    
    @always(busreq1)
    def eval_arb():
        busgr1.next = busreq1
    
    return eval_arb

def ADDRDECODER(IR2, REGDIR, REGIND, MEM, PCREL, REGINDOFF, IMM, iladr, MEMINDIR, **kwargs):
    '''Dekoder nacina adresiranja.
    
    Koriste se addr i Reg2 polje registra IR2.
    '''
    assert len(IR2) == 8
    
    @always(IR2)
    def eval_dec():
        addrfield = IR2.val[8:6]
        reg2field = IR2.val[2:0]
        for sig in REGDIR, REGIND, MEM, PCREL, REGINDOFF, IMM, iladr, MEMINDIR:
            sig.next = 0
        if addrfield == 0:
            REGDIR.next = 1
        elif addrfield == 1:
            REGIND.next = 1
        elif addrfield == 2:
            REGINDOFF.next = 1
        elif addrfield == 3:
            if reg2field == 0:
                MEM.next = 1
            elif reg2field == 1:
                MEM.next = 1
                MEMINDIR.next = 1
            elif reg2field == 2:
                PCREL.next = 1
            elif reg2field == 3:
                IMM.next = 1
            else:
                raise Exception("reg2field neprepoznat u addrdekoderu. %d" % reg2field.val)
        else:
            raise Exception("addrfield neprepoznat u addrdekoderu. %d" % addrfield.val)
    return eval_dec

def READWORDS(IR1, IR2, I1, I2, I3, **kwargs):
    '''Dekoder broja reci za citanje.
    
    
    '''
    assert len(IR1) == 8
    assert len(IR2) == 8
    assert len(I1) == 1
    assert len(I2) == 1
    assert len(I3) == 1
    
    @always_comb
    def eval_dec():
        if IR1[7] == False:
            I3.next = 0
            if IR1[6] == False:
                I1.next = 0
                I2.next = 0
                I3.next = 0
            else:
                I1.next = 1
                if IR1[5] == False:
                    I2.next = 1
                else:
                    I2.next = 0
        else:
            I1.next = 1
            addrfield = IR2[8:6]
            reg2field = IR2[2:0]
            if addrfield == 3:  #Ostala adresiranja
                I2.next = 1
                if reg2field == 2:  #PCrelativno
                    I3.next = 0
                else:
                    I3.next = 1
            else:
                I2.next = 0
                I3.next = 0
    return eval_dec
                
def INSTRDEC(IR1, IR2, ALU2OP, ALU1OP, PUSH, POP, MOVD, BNZ, JMP, 
                 JSR, RTI, RTS, IFLAGS, INT, JMPIND, HALT, ilop, **kwargs):
    assert len(IR1) == 8
    assert len(IR2) == 8
    sigs = (ALU2OP, ALU1OP, PUSH, POP, MOVD, BNZ, JMP, JSR, RTI, RTS, IFLAGS, INT, JMPIND, HALT, ilop)
    
    @always(IR1, IR2)
    def eval_dec():
        for sig in sigs:
            sig.next = 0
        if IR1[7] == 0:
            if IR1[6] == 0:
                opcode = IR1[6:0]
                if opcode == 0:
                    RTI.next = 1
                elif opcode == 1:
                    RTS.next =1
                elif opcode == 2 or opcode == 3 or opcode == 4 or opcode == 5:
                    IFFLAGS.next = 1
                elif opcode == 6:
                    HALT.next = 1
                else:
                    ilop.next = 1
            else:
                opcode = IR1[5:0]
                if IR[5] == 0:
                    if opcode == 0:
                        JMP.next = 1
                    elif opcode == 1:
                        JSR = 1
                    else:
                        ilop.next = 1
                else:
                    if opcode == 0:
                        INT.next = 1
                    elif opcode == 1:
                        BNZ = 1
                    else:
                        ilop.next = 1
        else:
            if IR1[7:0] == 0:
                ALU1OP.next = 1     #ASR
            elif IR1[8:0] == 255:
                addrfield = IR2[6:3]
                if addrfield == 1:
                    PUSH.next = 1
                elif addrfield == 2:
                    POP.next = 1
                elif addrfield == 3:
                    ALU1OP.next = 1      #INC
                elif addrfield == 4:
                    ALU1OP.next = 1     #DEC
                elif addrfield == 5:
                    JMPIND.next = 1
                else:
                    pass
            else:
                opcode = IR1[7:0]
                if opcode in (1,3,4):
                    ALU2OP.next = 1
                elif opcode == 2:
                    MOVD.next = 1
                else:
                    ilop.next = 1
    return eval_dec 

def ALUOPDECODER(aluopsel, IR1, IR2, ADD, AND, ASR, INC, DEC, MOVS, **kwargs): 
    '''Dekoder alu instrukcija unutar ALUops bloka.
    
    Na osnovu selektora 1 ili 2-adresnih instrukcija i opcode polja daje
    konkretnu alu operaciju koja treba da se obavi.
    '''
    assert len(IR1) == 8
    assert len(IR2) == 8
    assert len(aluopsel) == 1
    
    @always(aluopsel, IR1, IR2)
    def eval_dec():
        for sig in ADD, AND, ASR, INC, DEC, MOVS:
            sig.next = 0
        if aluopsel == 0: #1-adresna operacija
            addrfield = IR2[6:3]
            if addrfield == 0:
                ASR.next = 1
            elif addrfield == 1:
                pass
            elif addrfield == 2:
                pass
            elif addrfield == 3:
                INC.next = 1
            elif addrfield == 4:
                DEC.next = 1
            else:
                raise Exception("WTF? aluopdec daje %d kao opcod 1addrinst?" % addrfield)
        else:   #2adresne instrukcije
            addrfield = IR1[3:0]
            if addrfield == 1:
                MOVS.next = 1
            elif addrfield == 3:
                ADD.next = 1
            elif addrfield == 4:
                AND.next = 1
            else:
                pass
            
    return eval_dec

def ALUOPS_SPLIT(signals, PSWinta, PSWNZCV, PSWout, PSWin, PSWupdateIT, Ysignex, 
                 YselM1, Yin, ALUtrans, ALUout, ALUop, ALUinc, ALUdec, ALUadd, G, **kwargs):
    sigs = (G, ALUadd, ALUdec, ALUinc, ALUop,
                            ALUout, ALUtrans, Yin, YselM1,
                            Ysignex, PSWupdateIT, PSWin, PSWout,
                            PSWNZCV, PSWinta)
    @always(signals)
    def eval_splitter():
        for index, sig in enumerate(reversed(sigs)):
            if(signals[index] == True):
                sig.next = 1
            else:
                sig.next = 0
    return eval_splitter

def REGISTERS_SPLIT(signals, TEMP2swap, TEMP2out, TEMP2inLOW, 
                    TEMP2inHIGH, TEMPswap, TEMPout, TEMPinLOW, 
                    TEMPinHIGH, REGout, REGin, SPout, SPin, UNUSED_1, 
                    UNUSED_2, PCout, PCin, regsel2, IR4out, IR3out, 
                    IR2out, IR4in, IR3in, IR2in, IR1in, **kwargs):
    sigs = (IR1in,IR2in,IR3in,IR4in,IR2out,IR3out,IR4out,regsel2,PCin,
            PCout,UNUSED_2,UNUSED_1,SPin,SPout, REGin, REGout,TEMPinHIGH,TEMPinLOW,
            TEMPout,TEMPswap,TEMP2inHIGH,TEMP2inLOW,TEMP2out,TEMP2swap)
    @always(signals)
    def eval_splitter():
        for index, sig in (enumerate(reversed(sigs))):
            if(signals[index] == True):
                sig.next = 1
            else:
                sig.next = 0
    return eval_splitter

def IFACE_SPLIT(signals, read, write, MBRout, 
                MBRin, MARin, unused_sig, **kwargs):
    sigs = (unused_sig, MARin,  MBRin,MBRout, write, read)
    @always(signals)
    def eval_splitter():
        for index, sig in enumerate(reversed(sigs)):
            if(signals[index] == True):
                sig.next = 1
            else:
                sig.next = 0
    return eval_splitter

def ALU(X, Y, optrans, opadd, opand, opasr, 
        opinc, opdec, out16, out15, out, **kwargs):
    assert len(X) == 16
    assert len(Y) == 16
    assert len(out) == 16
    
    @always(X, Y, optrans, opadd, opand, opasr, opinc, opdec, out16, out15, out)
    def eval_alu():
        if optrans == 1:
            out.next = Y
        elif opadd == 1:    #ADD
            if X.val == None:
                return
            tmp = intbv(X.signed() + Y.signed())
            out.next = tmp[16:0]
            out16.next = tmp[16]
            out15.next = tmp[15]
        elif opand == 1:    #AND
            if X.val == None:
                return
            tmp = intbv(X & Y)
            out.next = tmp[16:0]
            out16.next = 0
            out15.next = tmp[15]
        elif opasr == 1:
            tmp = Y.val
            signbit = tmp[15]
            tmp = intbv(tmp >> 1)
            tmp[15] = signbit
            out.next = tmp[16:0]
            out16.next = 0
            out15.next = tmp[15]
        elif opinc == 1:
            tmp = intbv(Y.val + 1)
            out16.next = tmp[16]
            out15.next = tmp[15]
            out.next = tmp[16:0]
        elif opdec == 1:
            tmp = intbv(y.val - 1)
            out16.next = tmp[16]
            out15.next = tmp[15]
            out.next = tmp[16:0]
    return eval_alu
    
def NZCVKM(Y, X, out16, out, opadd, opinc, opdec, NZCV, **kwargs):
    assert len(Y) == 16
    assert len(X) == 16
    assert len(out) == 16
    assert len(out16) == 1
    assert len(NZCV) == 4
    
    @always(Y,X,out16, out, opadd, opinc, opdec)
    def eval_shit():
        if out[15] == True:
            N = True
        else:
            N = False
        if out == 0:
            Z = True
        else:
            Z = False
        if opdec == 1:
            if out16 == 1:
                C = False
            else:
                C = True
        else:
            if out16 == 1:
                C = True
            else:
                C = False
        if opadd == 1:
            if X.val == None:
                return
            if (X[15] == True and Y[15] == True and out[15] == True or
               X[15] == False and Y[15] == False and out[15] == True):
                V = True
            else:
                V = False
        elif opinc == 1:
            if Y[15] == True and out[15] == False:
                V = True
            else:
                V = False
        elif opdec == 1:
            if Y[15] == False and out[15] == True:
                V = True
            else:
                V = False
        else:
            V = False
        tmp = intbv(0)[4:0]
        tmp[3] = N
        tmp[2] = Z
        tmp[1] = C
        tmp[0] = V
        NZCV.next = tmp
    return eval_shit

def PSWREG(PSWinta, PSWNZCV, PSWout, PSWin, 
         PSWupdateIT, input, NZCV, clk, output, **kwargs):
    assert len(NZCV) == 4
    assert len(input) == 16
    assert len(output) == 16
    
    @always(clk)
    def eval_psw():
        '''Za sada trivijalna implementacija'''
        if (PSWNZCV) == 1:
            tmp = intbv(output.val)[16:0]
            tmp[4:0] = NZCV
            output.next = tmp
    
    return eval_psw

def TRIBUF(input, ctrl, output, **kwargs):
    assert isinstance(output, Tristate)
    assert len(input) == len(output)
    assert len(ctrl) == 1
    driver = output.driver()
    
    @always(input, ctrl)
    def eval_buf():
        if ctrl == 1:
            print "wtf", ctrl, ctrl.val
            driver.next = input
        else:
            driver.next = None
    return eval_buf        
    
def TRIINV(input, ctrl, output, **kwargs):
    assert isinstance(output, Tristate)
    assert len(input) == len(output)
    assert len(ctrl) == 1
    driver = output.driver()
    
    @always(input, ctrl)
    def eval_buf():
        if ctrl == 1:
            driver.next = (~input)[len(output):0]
        else:
            driver.next = None
    return eval_buf      

def INTERRUPTS(intr, int1, int2, int3, intNM, clk, inta2, 
               inta3, intaNM, M1, inta1, INTERRUPT, **kwargs):
    '''Za sada neimplementirani blok za prekide.'''
    @always(clk.posedge)
    def eval_intr():
        pass
    return eval_intr  
            
###########AUTOGENERATED###############
 
def KMOP(ALUREGOP, ALUADROP, PUSH, POP, MOVD, BNZ, JMP, JSR, RTI, RTS, RSFLAGS, INT, JMPIND, HALT, output, **kwargs):
    @always_comb    
    def eval_km():    
        if ALUREGOP == 1:        
            output.next = 0x2e            
        if ALUADROP == 1:        
            output.next = 0x31            
        if PUSH == 1:        
            output.next = 0x32            
        if POP == 1:        
            output.next = 0x3a            
        if MOVD == 1:        
            output.next = 0x41            
        if BNZ == 1:        
            output.next = 0x42            
        if JMP == 1:        
            output.next = 0x46            
        if JSR == 1:        
            output.next = 0x4b            
        if RTI == 1:        
            output.next = 0x57            
        if RTS == 1:        
            output.next = 0x66            
        if RSFLAGS == 1:        
            output.next = 0x6e            
        if INT == 1:        
            output.next = 0x70            
        if JMPIND == 1:        
            output.next = 0x73            
        if HALT == 1:        
            output.next = 0x76            
    return eval_km    
    
    
def KMADR1(REGDIR, REGIND, MEMADR, PCREL, REGINDOFF, IMM, output, **kwargs):
    @always_comb    
    def eval_km():    
        if REGDIR == 1:        
            output.next = 0x14            
        if REGIND == 1:        
            output.next = 0x25            
        if MEMADR == 1:        
            output.next = 0x15            
        if PCREL == 1:        
            output.next = 0x2b            
        if REGINDOFF == 1:        
            output.next = 0x26            
        if IMM == 1:        
            output.next = 0x28            
    return eval_km    
    
    
def KMADR2(STOREREGDIR, STOREREGIND, STOREMEMDIR, STOREMEMIND, STOREREGINDOFF, STOREPCREL, output, **kwargs):
    @always_comb    
    def eval_km():    
        if STOREREGDIR == 1:        
            output.next = 0x78            
        if STOREREGIND == 1:        
            output.next = 0x7a            
        if STOREMEMDIR == 1:        
            output.next = 0x7b            
        if STOREMEMIND == 1:        
            output.next = 0x7d            
        if STOREREGINDOFF == 1:        
            output.next = 0x85            
        if STOREPCREL == 1:        
            output.next = 0x87            
    return eval_km    

#######END AUTOGENERATED###############