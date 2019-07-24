/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "mosWrapper.h"

/* driver includes. */
#include "drvAudio.h"
#include "drvBDMA.h"
#include "drvMBX.h"
#include "drvPM.h"
#include "drvCPU.h"

#include "voc_task.h"
#include "apiVOC.h"

#define SRC_MSG(fmt, args...) MOS_DBG_PRINT(MOS_DBG_LEVEL_VP, "[SRC] " fmt, ##args)
#define SRC_ERROR(fmt, args...) MOS_DBG_ERROR("[SRC ERR] " fmt, ##args);

#define int16_t S16

static int nSrcInitFlag = 0;

static void (*chx_scale) (int16_t*, int16_t*, int, int, float*);
static void (*chx_scale48k) (int16_t*, int);
static int count; //frame count
static int i_ch;
//static int size; //buffer sample count
static int16_t *buff, *srcp;//, *dstp;
//static int16_t *buffer = NULL;
static int rd, wr, len;
static int order_count;
static int order_countN2;

#define NORM(x)    (0.95*x)

#if defined(CONFIG_VOC_SW_SRC)
#define MAX_FIR_TAB 200
static int16_t buffer[AUDIO_DMIC_NO*(MAX_FIR_TAB+1)];


#if 0 //origin
/*  nyq_rate = 16000
 *  width = .048
 *  cutoff = nyq_rate*(.470)
 *  n_taps,beta = kaiserord(52.0, width)
 *  taps = firwin(n_taps, cutoff, window=('kaiser',beta), scale=False, nyq=16000)
 */
static float res_coeff[] = {
NORM( 5.56474599092173695543e-05),
NORM(-2.53321265088761416948e-04),
NORM(-1.35923359693785721905e-04),
NORM( 3.21922560638170801323e-04),
NORM( 2.54991182471927743669e-04),
NORM(-3.74260230184551715218e-04),
NORM(-4.15261008515767078390e-04),
NORM( 3.96094109212138782445e-04),
NORM( 6.15547139431859104998e-04),
NORM(-3.71507865937306701928e-04),
NORM(-8.50199367699682926301e-04),
NORM( 2.83918613847235440460e-04),
NORM( 1.10842552372226901651e-03),
NORM(-1.17355793123527117269e-04),
NORM(-1.37389018291786792550e-03),
NORM(-1.42046552796574738868e-04),
NORM( 1.62466304078481989757e-03),
NORM( 5.04422887474439670188e-04),
NORM(-1.83357186663964197987e-03),
NORM(-9.74511951571839941250e-04),
NORM( 1.96899000630243459220e-03),
NORM( 1.55006442900116410584e-03),
NORM(-1.99605859966735027164e-03),
NORM(-2.22043535241519445281e-03),
NORM( 1.87831100476888268429e-03),
NORM( 2.96546791140357788447e-03),
NORM(-1.57963374971490104655e-03),
NORM(-3.75475837358475164271e-03),
NORM( 1.06646724737341426666e-03),
NORM( 4.54736350282131107647e-03),
NORM(-3.10123088727562554352e-04),
NORM(-5.29197172586529616944e-03),
NORM(-7.10924645181570209404e-04),
NORM( 5.92750660448908457678e-03),
NORM( 2.00892698420386973251e-03),
NORM(-6.38406389510353094313e-03),
NORM(-3.58507951431808391182e-03),
NORM( 6.58399652532481744621e-03),
NORM( 5.42821559477628650531e-03),
NORM(-6.44284343445819182583e-03),
NORM(-7.51409345517593807440e-03),
NORM( 5.86962257660647145768e-03),
NORM( 9.80536567488890509203e-03),
NORM(-4.76571919935853995087e-03),
NORM(-1.22522799814650269062e-02),
NORM( 3.02107268483395915298e-03),
NORM( 1.47941152881827953786e-02),
NORM(-5.05308099795018705533e-04),
NORM(-1.73613094460461879487e-02),
NORM(-2.95084812377707913708e-03),
NORM( 1.98781886292465834187e-02),
NORM( 7.59402684901306235937e-03),
NORM(-2.22661659694526996300e-02),
NORM(-1.38312224005614024108e-02),
NORM( 2.44472421560224285886e-02),
NORM( 2.24256155366290912956e-02),
NORM(-2.63476159199598006233e-02),
NORM(-3.50292800656630196054e-02),
NORM( 2.79011996364845783758e-02),
NORM( 5.59936759078231982567e-02),
NORM(-2.90528358873306941090e-02),
NORM(-1.01416991398681571868e-01),
NORM( 2.97610249430284891370e-02),
NORM( 3.16733373544636265251e-01),
NORM( 4.69999999999999973355e-01),
NORM( 3.16733373544636265251e-01),
NORM( 2.97610249430284891370e-02),
NORM(-1.01416991398681571868e-01),
NORM(-2.90528358873306941090e-02),
NORM( 5.59936759078231982567e-02),
NORM( 2.79011996364845783758e-02),
NORM(-3.50292800656630196054e-02),
NORM(-2.63476159199598006233e-02),
NORM( 2.24256155366290912956e-02),
NORM( 2.44472421560224285886e-02),
NORM(-1.38312224005614024108e-02),
NORM(-2.22661659694526996300e-02),
NORM( 7.59402684901306235937e-03),
NORM( 1.98781886292465834187e-02),
NORM(-2.95084812377707913708e-03),
NORM(-1.73613094460461879487e-02),
NORM(-5.05308099795018705533e-04),
NORM( 1.47941152881827953786e-02),
NORM( 3.02107268483395915298e-03),
NORM(-1.22522799814650269062e-02),
NORM(-4.76571919935853995087e-03),
NORM( 9.80536567488890509203e-03),
NORM( 5.86962257660647145768e-03),
NORM(-7.51409345517593807440e-03),
NORM(-6.44284343445819182583e-03),
NORM( 5.42821559477628650531e-03),
NORM( 6.58399652532481744621e-03),
NORM(-3.58507951431808391182e-03),
NORM(-6.38406389510353094313e-03),
NORM( 2.00892698420386973251e-03),
NORM( 5.92750660448908457678e-03),
NORM(-7.10924645181570209404e-04),
NORM(-5.29197172586529616944e-03),
NORM(-3.10123088727562554352e-04),
NORM( 4.54736350282131107647e-03),
NORM( 1.06646724737341426666e-03),
NORM(-3.75475837358475164271e-03),
NORM(-1.57963374971490104655e-03),
NORM( 2.96546791140357788447e-03),
NORM( 1.87831100476888268429e-03),
NORM(-2.22043535241519445281e-03),
NORM(-1.99605859966735027164e-03),
NORM( 1.55006442900116410584e-03),
NORM( 1.96899000630243459220e-03),
NORM(-9.74511951571839941250e-04),
NORM(-1.83357186663964197987e-03),
NORM( 5.04422887474439670188e-04),
NORM( 1.62466304078481989757e-03),
NORM(-1.42046552796574738868e-04),
NORM(-1.37389018291786792550e-03),
NORM(-1.17355793123527117269e-04),
NORM( 1.10842552372226901651e-03),
NORM( 2.83918613847235440460e-04),
NORM(-8.50199367699682926301e-04),
NORM(-3.71507865937306701928e-04),
NORM( 6.15547139431859104998e-04),
NORM( 3.96094109212138782445e-04),
NORM(-4.15261008515767078390e-04),
NORM(-3.74260230184551715218e-04),
NORM( 2.54991182471927743669e-04),
NORM( 3.21922560638170801323e-04),
NORM(-1.35923359693785721905e-04),
NORM(-2.53321265088761416948e-04),
NORM( 5.56474599092173695543e-05),
};

#else
/*
Dec2 from 32KHz to 16KHz
Equiripple
Fs: 32000 Hz
Fpass: 7200 Hz
Fstop: 8000 Hz
Apass: 0.1 dB
Astop: 100 dB
*/

static float res_coeff[] = {
NORM(-2.599452112387205e-005),
NORM( 3.844149401808133e-005),
NORM( 4.072985393173553e-004),
NORM( 1.041743324219992e-003),
NORM( 1.368179757924457e-003),
NORM( 8.097231058391475e-004),
NORM(-2.690914325790210e-004),
NORM(-7.058905811941607e-004),
NORM(-5.400470573040202e-005),
NORM( 6.256366743293370e-004),
NORM( 2.433571408046240e-004),
NORM(-5.749253195428475e-004),
NORM(-4.167115579618169e-004),
NORM( 5.200525602290306e-004),
NORM( 6.034102238155819e-004),
NORM(-4.304970731862837e-004),
NORM(-8.016562183298039e-004),
NORM( 2.851520844944683e-004),
NORM( 9.967363321208582e-004),
NORM(-7.200186905547162e-005),
NORM(-1.168864488601825e-003),
NORM(-2.149996159545778e-004),
NORM( 1.292744499989117e-003),
NORM( 5.712795590573864e-004),
NORM(-1.346192146105397e-003),
NORM(-9.905808716944987e-004),
NORM( 1.300469184342473e-003),
NORM( 1.451187152154126e-003),
NORM(-1.137303564266771e-003),
NORM(-1.931768663487603e-003),
NORM( 8.360682924771020e-004),
NORM( 2.399624612246135e-003),
NORM(-3.842354002936198e-004),
NORM(-2.815675901647799e-003),
NORM(-2.204925837145416e-004),
NORM( 3.139555661777214e-003),
NORM( 9.738027013637660e-004),
NORM(-3.326651882441134e-003),
NORM(-1.860651902310509e-003),
NORM( 3.330745344927486e-003),
NORM( 2.853172141294577e-003),
NORM(-3.107995665711901e-003),
NORM(-3.910623927175391e-003),
NORM( 2.621384225257308e-003),
NORM( 4.983488419962189e-003),
NORM(-1.838240205165815e-003),
NORM(-6.009260642630532e-003),
NORM( 7.366246134312183e-004),
NORM( 6.916573291873019e-003),
NORM( 6.951829546049604e-004),
NORM(-7.623705643215586e-003),
NORM(-2.453565022228191e-003),
NORM( 8.043577785711294e-003),
NORM( 4.520883635811419e-003),
NORM(-8.084568068667194e-003),
NORM(-6.866500646760791e-003),
NORM( 7.648561664696634e-003),
NORM( 9.444515046089409e-003),
NORM(-6.631921781588675e-003),
NORM(-1.219592804352530e-002),
NORM( 4.921157255577565e-003),
NORM( 1.504982357902645e-002),
NORM(-2.384954535047154e-003),
NORM(-1.792691096854647e-002),
NORM(-1.145596819814396e-003),
NORM( 2.074121361684722e-002),
NORM( 5.913647870148924e-003),
NORM(-2.340617810073476e-002),
NORM(-1.232271292752504e-002),
NORM( 2.583574447917471e-002),
NORM( 2.113080716984307e-002),
NORM(-2.794918778416188e-002),
NORM(-3.398383626776133e-002),
NORM( 2.967454143349835e-002),
NORM( 5.522614993770386e-002),
NORM(-3.095192746073385e-002),
NORM(-1.009477734168768e-001),
NORM( 3.173728601950669e-002),
NORM( 3.165755180970835e-001),
NORM( 4.679977776248557e-001),
NORM( 3.165755180970835e-001),
NORM( 3.173728601950669e-002),
NORM(-1.009477734168768e-001),
NORM(-3.095192746073385e-002),
NORM( 5.522614993770386e-002),
NORM( 2.967454143349835e-002),
NORM(-3.398383626776133e-002),
NORM(-2.794918778416188e-002),
NORM( 2.113080716984307e-002),
NORM( 2.583574447917471e-002),
NORM(-1.232271292752504e-002),
NORM(-2.340617810073476e-002),
NORM( 5.913647870148924e-003),
NORM( 2.074121361684722e-002),
NORM(-1.145596819814396e-003),
NORM(-1.792691096854647e-002),
NORM(-2.384954535047154e-003),
NORM( 1.504982357902645e-002),
NORM( 4.921157255577565e-003),
NORM(-1.219592804352530e-002),
NORM(-6.631921781588675e-003),
NORM( 9.444515046089409e-003),
NORM( 7.648561664696634e-003),
NORM(-6.866500646760791e-003),
NORM(-8.084568068667194e-003),
NORM( 4.520883635811419e-003),
NORM( 8.043577785711294e-003),
NORM(-2.453565022228191e-003),
NORM(-7.623705643215586e-003),
NORM( 6.951829546049604e-004),
NORM( 6.916573291873019e-003),
NORM( 7.366246134312183e-004),
NORM(-6.009260642630532e-003),
NORM(-1.838240205165815e-003),
NORM( 4.983488419962189e-003),
NORM( 2.621384225257308e-003),
NORM(-3.910623927175391e-003),
NORM(-3.107995665711901e-003),
NORM( 2.853172141294577e-003),
NORM( 3.330745344927486e-003),
NORM(-1.860651902310509e-003),
NORM(-3.326651882441134e-003),
NORM( 9.738027013637660e-004),
NORM( 3.139555661777214e-003),
NORM(-2.204925837145416e-004),
NORM(-2.815675901647799e-003),
NORM(-3.842354002936198e-004),
NORM( 2.399624612246135e-003),
NORM( 8.360682924771020e-004),
NORM(-1.931768663487603e-003),
NORM(-1.137303564266771e-003),
NORM( 1.451187152154126e-003),
NORM( 1.300469184342473e-003),
NORM(-9.905808716944987e-004),
NORM(-1.346192146105397e-003),
NORM( 5.712795590573864e-004),
NORM( 1.292744499989117e-003),
NORM(-2.149996159545778e-004),
NORM(-1.168864488601825e-003),
NORM(-7.200186905547162e-005),
NORM( 9.967363321208582e-004),
NORM( 2.851520844944683e-004),
NORM(-8.016562183298039e-004),
NORM(-4.304970731862837e-004),
NORM( 6.034102238155819e-004),
NORM( 5.200525602290306e-004),
NORM(-4.167115579618169e-004),
NORM(-5.749253195428475e-004),
NORM( 2.433571408046240e-004),
NORM( 6.256366743293370e-004),
NORM(-5.400470573040202e-005),
NORM(-7.058905811941607e-004),
NORM(-2.690914325790210e-004),
NORM( 8.097231058391475e-004),
NORM( 1.368179757924457e-003),
NORM( 1.041743324219992e-003),
NORM( 4.072985393173553e-004),
NORM( 3.844149401808133e-005),
NORM(-2.599452112387205e-005),
};

#endif //0

#else

#define MAX_FIR_TAB 251
static int16_t buffer[AUDIO_DMIC_NO*(MAX_FIR_TAB+1)];

/*
Dec3 from 48KHz to 16KHz
Equiripple
Fs: 48000 Hz
Fpass: 7200 Hz
Fstop: 8000 Hz
Apass: 0.1 dB
Astop: 100 dB
*/

static float res_coeff[] = {
NORM( 5.693911973727670e-006),
NORM( 7.021054144194635e-005),
NORM( 2.212154347225401e-004),
NORM( 4.678239760218696e-004),
NORM( 7.408950309292430e-004),
NORM( 9.065909044627795e-004),
NORM( 8.319479190373409e-004),
NORM( 4.841980271218904e-004),
NORM(-9.260239441624673e-006),
NORM(-4.046206373268804e-004),
NORM(-4.863924073259136e-004),
NORM(-2.237804171806503e-004),
NORM( 1.838486403972386e-004),
NORM( 4.306434717154399e-004),
NORM( 3.262944145919621e-004),
NORM(-4.890212356532744e-005),
NORM(-3.916511631021967e-004),
NORM(-4.105719743071261e-004),
NORM(-7.182374408510616e-005),
NORM( 3.524325260173760e-004),
NORM( 4.948845503686001e-004),
NORM( 2.064727046996568e-004),
NORM(-2.923533476033795e-004),
NORM(-5.732165894787602e-004),
NORM(-3.629473031997656e-004),
NORM( 1.957311266819340e-004),
NORM( 6.312997388760439e-004),
NORM( 5.366139603981577e-004),
NORM(-5.542304033321175e-005),
NORM(-6.565718119970857e-004),
NORM(-7.206196351223714e-004),
NORM(-1.350536683308464e-004),
NORM( 6.317548782841730e-004),
NORM( 8.986018177264548e-004),
NORM( 3.720889574965388e-004),
NORM(-5.457881688682599e-004),
NORM(-1.055225941362375e-003),
NORM(-6.506567948078591e-004),
NORM( 3.870780634414114e-004),
NORM( 1.170296568598176e-003),
NORM( 9.582242439191704e-004),
NORM(-1.498602088437524e-004),
NORM(-1.224587682427835e-003),
NORM(-1.279110609834822e-003),
NORM(-1.685585724301156e-004),
NORM( 1.197808179913616e-003),
NORM( 1.592151298818527e-003),
NORM( 5.648826779358573e-004),
NORM(-1.071307805592681e-003),
NORM(-1.871991627485146e-003),
NORM(-1.029222002856071e-003),
NORM( 8.298609016544608e-004),
NORM( 2.090628647261267e-003),
NORM( 1.545422811527024e-003),
NORM(-4.622524964179427e-004),
NORM(-2.217619358312503e-003),
NORM(-2.089730726072423e-003),
NORM(-3.585254093284149e-005),
NORM( 2.223268115676653e-003),
NORM( 2.632944815972629e-003),
NORM( 6.626770913797293e-004),
NORM(-2.078323016001229e-003),
NORM(-3.139226349764737e-003),
NORM(-1.407753482826818e-003),
NORM( 1.757107288444220e-003),
NORM( 3.568329426457807e-003),
NORM( 2.252495143074916e-003),
NORM(-1.237933275217855e-003),
NORM(-3.875507828623155e-003),
NORM(-3.168619651053574e-003),
NORM( 5.057196794447699e-004),
NORM( 4.013653324925550e-003),
NORM( 4.118570043014271e-003),
NORM( 4.468426909775308e-004),
NORM(-3.934668092978879e-003),
NORM(-5.055709592473773e-003),
NORM(-1.618137099359036e-003),
NORM( 3.590887352724151e-003),
NORM( 5.924738773986159e-003),
NORM( 2.996716493907317e-003),
NORM(-2.936526789408071e-003),
NORM(-6.662722083912889e-003),
NORM(-4.561656018780191e-003),
NORM( 1.927533789216636e-003),
NORM( 7.198732912702074e-003),
NORM( 6.282195773764805e-003),
NORM(-5.216010850448492e-004),
NORM(-7.453936895449887e-003),
NORM(-8.119004131236349e-003),
NORM(-1.324627110569768e-003),
NORM( 7.339044204033412e-003),
NORM( 1.002483215500585e-002),
NORM( 3.660168206907480e-003),
NORM(-6.749766010637820e-003),
NORM(-1.194661148095456e-002),
NORM(-6.550470975219195e-003),
NORM( 5.555398736200127e-003),
NORM( 1.382732367222850e-002),
NORM( 1.009804869740821e-002),
NORM(-3.574874054215181e-003),
NORM(-1.560846367626120e-002),
NORM(-1.448583586126818e-002),
NORM( 5.229273050490191e-004),
NORM( 1.723251918055233e-002),
NORM( 2.007579196871261e-002),
NORM( 4.120894152647547e-003),
NORM(-1.864536200369800e-002),
NORM(-2.766712981962494e-002),
NORM(-1.148120736550549e-002),
NORM( 1.979885228214453e-002),
NORM( 3.933289620704607e-002),
NORM( 2.460759105060908e-002),
NORM(-2.065294940318092e-002),
NORM(-6.226029522918286e-002),
NORM(-5.573922409790814e-002),
NORM( 2.117775730006895e-002),
NORM( 1.471338228633859e-001),
NORM( 2.643277086576704e-001),
NORM( 3.119785647468834e-001),
NORM( 2.643277086576704e-001),
NORM( 1.471338228633859e-001),
NORM( 2.117775730006895e-002),
NORM(-5.573922409790814e-002),
NORM(-6.226029522918286e-002),
NORM(-2.065294940318092e-002),
NORM( 2.460759105060908e-002),
NORM( 3.933289620704607e-002),
NORM( 1.979885228214453e-002),
NORM(-1.148120736550549e-002),
NORM(-2.766712981962494e-002),
NORM(-1.864536200369800e-002),
NORM( 4.120894152647547e-003),
NORM( 2.007579196871261e-002),
NORM( 1.723251918055233e-002),
NORM( 5.229273050490191e-004),
NORM(-1.448583586126818e-002),
NORM(-1.560846367626120e-002),
NORM(-3.574874054215181e-003),
NORM( 1.009804869740821e-002),
NORM( 1.382732367222850e-002),
NORM( 5.555398736200127e-003),
NORM(-6.550470975219195e-003),
NORM(-1.194661148095456e-002),
NORM(-6.749766010637820e-003),
NORM( 3.660168206907480e-003),
NORM( 1.002483215500585e-002),
NORM( 7.339044204033412e-003),
NORM(-1.324627110569768e-003),
NORM(-8.119004131236349e-003),
NORM(-7.453936895449887e-003),
NORM(-5.216010850448492e-004),
NORM( 6.282195773764805e-003),
NORM( 7.198732912702074e-003),
NORM( 1.927533789216636e-003),
NORM(-4.561656018780191e-003),
NORM(-6.662722083912889e-003),
NORM(-2.936526789408071e-003),
NORM( 2.996716493907317e-003),
NORM( 5.924738773986159e-003),
NORM( 3.590887352724151e-003),
NORM(-1.618137099359036e-003),
NORM(-5.055709592473773e-003),
NORM(-3.934668092978879e-003),
NORM( 4.468426909775308e-004),
NORM( 4.118570043014271e-003),
NORM( 4.013653324925550e-003),
NORM( 5.057196794447699e-004),
NORM(-3.168619651053574e-003),
NORM(-3.875507828623155e-003),
NORM(-1.237933275217855e-003),
NORM( 2.252495143074916e-003),
NORM( 3.568329426457807e-003),
NORM( 1.757107288444220e-003),
NORM(-1.407753482826818e-003),
NORM(-3.139226349764737e-003),
NORM(-2.078323016001229e-003),
NORM( 6.626770913797293e-004),
NORM( 2.632944815972629e-003),
NORM( 2.223268115676653e-003),
NORM(-3.585254093284149e-005),
NORM(-2.089730726072423e-003),
NORM(-2.217619358312503e-003),
NORM(-4.622524964179427e-004),
NORM( 1.545422811527024e-003),
NORM( 2.090628647261267e-003),
NORM( 8.298609016544608e-004),
NORM(-1.029222002856071e-003),
NORM(-1.871991627485146e-003),
NORM(-1.071307805592681e-003),
NORM( 5.648826779358573e-004),
NORM( 1.592151298818527e-003),
NORM( 1.197808179913616e-003),
NORM(-1.685585724301156e-004),
NORM(-1.279110609834822e-003),
NORM(-1.224587682427835e-003),
NORM(-1.498602088437524e-004),
NORM( 9.582242439191704e-004),
NORM( 1.170296568598176e-003),
NORM( 3.870780634414114e-004),
NORM(-6.506567948078591e-004),
NORM(-1.055225941362375e-003),
NORM(-5.457881688682599e-004),
NORM( 3.720889574965388e-004),
NORM( 8.986018177264548e-004),
NORM( 6.317548782841730e-004),
NORM(-1.350536683308464e-004),
NORM(-7.206196351223714e-004),
NORM(-6.565718119970857e-004),
NORM(-5.542304033321175e-005),
NORM( 5.366139603981577e-004),
NORM( 6.312997388760439e-004),
NORM( 1.957311266819340e-004),
NORM(-3.629473031997656e-004),
NORM(-5.732165894787602e-004),
NORM(-2.923533476033795e-004),
NORM( 2.064727046996568e-004),
NORM( 4.948845503686001e-004),
NORM( 3.524325260173760e-004),
NORM(-7.182374408510616e-005),
NORM(-4.105719743071261e-004),
NORM(-3.916511631021967e-004),
NORM(-4.890212356532744e-005),
NORM( 3.262944145919621e-004),
NORM( 4.306434717154399e-004),
NORM( 1.838486403972386e-004),
NORM(-2.237804171806503e-004),
NORM(-4.863924073259136e-004),
NORM(-4.046206373268804e-004),
NORM(-9.260239441624673e-006),
NORM( 4.841980271218904e-004),
NORM( 8.319479190373409e-004),
NORM( 9.065909044627795e-004),
NORM( 7.408950309292430e-004),
NORM( 4.678239760218696e-004),
NORM( 2.212154347225401e-004),
NORM( 7.021054144194635e-005),
NORM( 5.693911973727670e-006),
};

#endif //CONFIG_VOC_SW_SRC



static int res_order = sizeof(res_coeff)/sizeof(float);

#if 0

static void ch6_scale_wAec(int16_t* outp, int16_t* pcms, int count, int order, float* coeff) {
    int i, j;
    float a0, a1, a2, a3, a4, a5;
    float* tap = coeff;
    int16_t* s = pcms;
    for (i = 0; i < count; i += 2) {
        tap = coeff;
        s = pcms;
        a0 = a1 = a2 = a3 = a4 = a5 = 0;

        for (j = 0; j < order-1; j++) {
            a0 += *tap * (float)*s++;
            a1 += *tap * (float)*s++;
            a2 += *tap * (float)*s++;
            a3 += *tap * (float)*s++;
            s+=2;
            tap++;
        }
        a0 += *tap * (float)*s++;
        a1 += *tap * (float)*s++;
        a2 += *tap * (float)*s++;
        a3 += *tap * (float)*s++;
        a4 = (float)*s++;
        a5 = (float)*s++;

        pcms += 6*2;
        *outp++ = (int16_t)a0;
        *outp++ = (int16_t)a1;
        *outp++ = (int16_t)a2;
        *outp++ = (int16_t)a3;
        *outp++ = (int16_t)a4;
        *outp++ = (int16_t)a5;
    }
}


static void ch6_scale(int16_t* outp, int16_t* pcms, int count, int order, float* coeff) {
    int i, j;
    float a0, a1, a2, a3, a4, a5;
    float* tap = coeff;
    int16_t* s = pcms;
    for (i = 0; i < count; i += 2) {
        tap = coeff;
        s = pcms;
        a0 = a1 = a2 = a3 = a4 = a5 = 0;
        for (j = 0; j < order; j++) {
            a0 += *tap * (float)*s++;
            a1 += *tap * (float)*s++;
            a2 += *tap * (float)*s++;
            a3 += *tap * (float)*s++;
            a4 += *tap * (float)*s++;
            a5 += *tap * (float)*s++;
            tap++;
        }
        pcms += 6*2;
        *outp++ = (int16_t)a0;
        *outp++ = (int16_t)a1;
        *outp++ = (int16_t)a2;
        *outp++ = (int16_t)a3;
        *outp++ = (int16_t)a4;
        *outp++ = (int16_t)a5;
    }
}


static void ch4_scale(int16_t* outp, int16_t* pcms, int count, int order, float* coeff) {
    int i, j;
    float a0, a1, a2, a3;
    float* tap = coeff;
    int16_t* s = pcms;
    for (i = 0; i < count; i += 2) {
        tap = coeff;
        s = pcms;
        a0 = a1 = a2 = a3 = 0;
        for (j = 0; j < order; j++) {
            a0 += *tap * (float)*s++;
            a1 += *tap * (float)*s++;
            a2 += *tap * (float)*s++;
            a3 += *tap * (float)*s++;
            tap++;
        }
        pcms += 4*2;
        *outp++ = (int16_t)a0;
        *outp++ = (int16_t)a1;
        *outp++ = (int16_t)a2;
        *outp++ = (int16_t)a3;
    }
}

static void ch2_scale(int16_t* outp, int16_t* pcms, int count, int order, float* coeff) {
    int i, j;
    float a0, a1;
    float* tap = coeff;
    int16_t* s = pcms;
    for (i = 0; i < count; i += 2) {
        tap = coeff;
        s = pcms;
        a0 = a1 = 0;
        for (j = 0; j < order; j++) {
            a0 += *tap * (float)*s++;
            a1 += *tap * (float)*s++;
            tap++;
        }
        pcms += 2*2;
        *outp++ = (int16_t)a0;
        *outp++ = (int16_t)a1;
    }
}

#if 0
int main(int argc, char** argv) {
    if (argc != 3) {
        printf("%s wavefi.wav wavefo.wav\n", argv[0]);
        return 0;
    }
    struct wavefile* wavfi = NULL;
    struct wavefile* wavfo = NULL;
    wavfi = wave_open(argv[1], WAVE_RDONLY);
    wavfo = wave_open(argv[2], WAVE_WRONLY);

    do
    if (wavfi && wavfo) {
        if ((wavfi->fmt.channels%2) != 0) {
            printf("bad channels number !\n");
            break;
        }
        if (wavfi->fmt.bits != 16) {
            printf("bad bps !\n");
            break;
        }
        if (wavfi->fmt.rate != 32000) {
            printf("bad sampling rate !\n");
            break;
        }
        void (*chx_scale) (int16_t*, int16_t*, int, int, float*);
        int count = 1024;
        int i_ch = wavfi->fmt.channels;
        int size = ((count*3)+(res_order-1))*i_ch;
        int16_t *buff, *srcp, *dstp;
        int16_t *buffer = malloc(size*sizeof(int16_t));
        chx_scale = i_ch==2 ? ch2_scale : ch4_scale;
        srcp = buffer;
        buff = buffer+(res_order-1)*i_ch;
        dstp = buff+(count*2)*i_ch;
        wavfo->fmt.channels = i_ch;
        wavfo->fmt.bits = 16;
        wavfo->fmt.rate = wavfi->fmt.rate/2;
        wavfo->fmt.block = wavfo->fmt.channels*wavfo->fmt.bits/8;
        wavfo->fmt.byte_rate = wavfo->fmt.block*wavfo->fmt.rate;
        memset(srcp, 0, (res_order-1)*i_ch*sizeof(int16_t));
        int rdsz = count*2;
        while (0 < (rdsz = wave_read(wavfi, buff, rdsz))) {
            chx_scale(dstp, srcp, rdsz, res_order, res_coeff);
            memcpy(srcp, srcp+rdsz*i_ch, (res_order-1)*i_ch*sizeof(int16_t));
            wave_write(wavfo, dstp, rdsz/2);
        }
    }
    while (0);

    if (wavfi)
        wave_close(wavfi);
    if (wavfo)
        wave_close(wavfo);
    return 0;
}
#endif


BOOL MApi_VOC_SrcProcess(S16 *ps16Buffer, U32 *pu32Count)
{
  if (nSrcInitFlag == 0)
  {
    i_ch = MDrv_AUD_GetMicNo();
    count = *pu32Count/i_ch;
    size = ((count)+(res_order-1))*i_ch;
    buffer = malloc(size*sizeof(int16_t));
    if (!buffer)
    {
      SRC_ERROR("MApi_VOC_SrcProcess: Init malloc failed\r\n");
      return FALSE;
    }

    SRC_MSG("MApi_VOC_SrcProcess: malloc(%d) = 0x%x\r\n", size, buffer);

    memset(buffer, 0, size*sizeof(int16_t));
    chx_scale = (i_ch == 2) ? ch2_scale : ch4_scale;

    srcp = buffer;
    buff = buffer+(res_order-1)*i_ch;
    //dstp = buff+(count*2)*i_ch;
    nSrcInitFlag = 1;

    //return TRUE;
  }

  if (nSrcInitFlag)
  {
    memcpy(buff, ps16Buffer, count*i_ch*sizeof(int16_t));
    chx_scale(ps16Buffer, srcp, count, res_order, res_coeff);
    memcpy(srcp, srcp+count*i_ch, (res_order-1)*i_ch*sizeof(int16_t));

    *pu32Count = *pu32Count/2;

    return TRUE;
  }

  return FALSE;
}
#else

#define __SATURATION__
#ifdef __SATURATION__
#define MAX_S16_F 32767.0//0x7FFF
#define MIN_S16_F -32768.0//0x8000
#endif

#if defined(CONFIG_AUD_AEC) && defined(CONFIG_AUD_REFEXT)
static void ch6_scale_wAec(int16_t* outp, int16_t* pcms, int offset, int order, float* coeff) {
  int i;
  float a[6];
  float* tap = coeff;
  int16_t* s = pcms;
  for (i = 0; i < 6; i++)
    a[i] = 0;

  for (i = 0; i < order-1; i++) {
    a[0] += *tap * (float)*s++;
    a[1] += *tap * (float)*s++;
    a[2] += *tap * (float)*s++;
    a[3] += *tap * (float)*s++;
    tap++;
    s+=2;
  }

  a[0] += *tap * (float)*s++;
  a[1] += *tap * (float)*s++;
  a[2] += *tap * (float)*s++;
  a[3] += *tap * (float)*s++;
  a[4] = (float)*s++;
  a[5] = (float)*s++;

#ifdef __SATURATION__
  for (i = 0; i < 6; i++)
  {
    if(a[i]>MAX_S16_F)
      a[i]=MAX_S16_F;
    else if(a[i]<MIN_S16_F)
      a[i]=MIN_S16_F;
  }
#endif

  *(outp+offset+0) = (int16_t)a[0];
  *(outp+offset+1) = (int16_t)a[1];
  *(outp+offset+2) = (int16_t)a[2];
  *(outp+offset+3) = (int16_t)a[3];
  *(outp+offset+4) = (int16_t)a[4];
  *(outp+offset+5) = (int16_t)a[5];
}
#endif
static void ch6_scale(int16_t* outp, int16_t* pcms, int offset, int order, float* coeff) {
  int i;
  float a[6];
  float* tap = coeff;
  int16_t* s = pcms;
  for (i = 0; i < 6; i++)
    a[i] = 0;

  for (i = 0; i < order; i++) {
    a[0] += *tap * (float)*s++;
    a[1] += *tap * (float)*s++;
    a[2] += *tap * (float)*s++;
    a[3] += *tap * (float)*s++;
    a[4] += *tap * (float)*s++;
    a[5] += *tap * (float)*s++;
    tap++;
  }

#ifdef __SATURATION__
  for (i = 0; i < 6; i++)
  {
    if(a[i]>MAX_S16_F)
      a[i]=MAX_S16_F;
    else if(a[i]<MIN_S16_F)
      a[i]=MIN_S16_F;
  }
#endif

  *(outp+offset+0) = (int16_t)a[0];
  *(outp+offset+1) = (int16_t)a[1];
  *(outp+offset+2) = (int16_t)a[2];
  *(outp+offset+3) = (int16_t)a[3];
  *(outp+offset+4) = (int16_t)a[4];
  *(outp+offset+5) = (int16_t)a[5];
}

static void ch4_scale(int16_t* outp, int16_t* pcms, int offset, int order, float* coeff) {
  int i;
  float a[4];
  float* tap = coeff;
  int16_t* s = pcms;
  for (i = 0; i < 4; i++)
    a[i] = 0;

  for (i = 0; i < order; i++) {
    a[0] += *tap * (float)*s++;
    a[1] += *tap * (float)*s++;
    a[2] += *tap * (float)*s++;
    a[3] += *tap * (float)*s++;
    tap++;
  }

#ifdef __SATURATION__
  for (i = 0; i < 4; i++)
  {
    if(a[i]>MAX_S16_F)
      a[i]=MAX_S16_F;
    else if(a[i]<MIN_S16_F)
      a[i]=MIN_S16_F;
  }
#endif

  *(outp+offset+0) = (int16_t)a[0];
  *(outp+offset+1) = (int16_t)a[1];
  *(outp+offset+2) = (int16_t)a[2];
  *(outp+offset+3) = (int16_t)a[3];
}

static void ch2_scale(int16_t* outp, int16_t* pcms, int offset, int order, float* coeff) {
  int i;
  float a[2];
  float* tap = coeff;
  int16_t* s = pcms;
  for (i = 0; i < 2; i++)
    a[i] = 0;

  for (i = 0; i < order; i++) {
    a[0] += *tap * (float)*s++;
    a[1] += *tap * (float)*s++;
    tap++;
  }

#ifdef __SATURATION__
  for (i = 0; i < 2; i++)
  {
    if(a[i]>MAX_S16_F)
      a[i]=MAX_S16_F;
    else if(a[i]<MIN_S16_F)
      a[i]=MIN_S16_F;
  }
#endif

  *(outp+offset+0) = (int16_t)a[0];
  *(outp+offset+1) = (int16_t)a[1];
}

#if defined(CONFIG_AUD_AEC) && defined(CONFIG_AUD_REFEXT)
static void ch6_scale_wAec48k(int16_t* outp, int offset)
{
  int i, j, k;
  float a[6];
  float* tap = res_coeff;
  for (i = 0; i < 6; i++)
    a[i] = 0;
/*
  for (i = 0; i < order-1; i++) {
    a[0] += *tap * (float)*s++;
    a[1] += *tap * (float)*s++;
    a[2] += *tap * (float)*s++;
    a[3] += *tap * (float)*s++;
    tap++;
    s+=2;
  }

  a[0] += *tap * (float)*s++;
  a[1] += *tap * (float)*s++;
  a[2] += *tap * (float)*s++;
  a[3] += *tap * (float)*s++;
  a[4] = (float)*s++;
  a[5] = (float)*s++;
*/

  for (i = 0; i < order_count; i++) {
    j = (rd+i*6)%len;
    k = (rd+ (res_order - 1- i)*6)%len;
    a[0] += *tap * ((float)*(buffer + (j  )) + (float)*(buffer + (k   )));
    a[1] += *tap * ((float)*(buffer + (j+1)) + (float)*(buffer + (k+1 )));
    a[2] += *tap * ((float)*(buffer + (j+2)) + (float)*(buffer + (k+2 )));
    a[3] += *tap * ((float)*(buffer + (j+3)) + (float)*(buffer + (k+3 )));

    tap++;
  }

  if (order_countN2)
  {
    j = (rd+i*6)%len;
    a[0] += *tap * (float)*(buffer + (j  ));
    a[1] += *tap * (float)*(buffer + (j+1));
    a[2] += *tap * (float)*(buffer + (j+2));
    a[3] += *tap * (float)*(buffer + (j+3));
  }

  j = (rd+ (res_order - 1)*6)%len;
  a[4] = (float)*(buffer + (j+4));
  a[5] = (float)*(buffer + (j+5));

  rd=(rd+6*3)%len;

#ifdef __SATURATION__
  for (i = 0; i < 4; i++)
  {
    if(a[i]>MAX_S16_F)
      a[i]=MAX_S16_F;
    else if(a[i]<MIN_S16_F)
      a[i]=MIN_S16_F;
  }
#endif

  *(outp+offset+0) = (int16_t)a[0];
  *(outp+offset+1) = (int16_t)a[1];
  *(outp+offset+2) = (int16_t)a[2];
  *(outp+offset+3) = (int16_t)a[3];
  *(outp+offset+4) = (int16_t)a[4];
  *(outp+offset+5) = (int16_t)a[5];
}
#endif
static void ch6_scale48k(int16_t* outp, int offset)
{
  int i, j, k;
  float a[6];
  float* tap = res_coeff;
  for (i = 0; i < 6; i++)
    a[i] = 0;
/*
  for (i = 0; i < order; i++) {
    a[0] += *tap * (float)*(buffer + (rd+i*6  )%len);
    a[1] += *tap * (float)*(buffer + (rd+i*6+1)%len);
    a[2] += *tap * (float)*(buffer + (rd+i*6+2)%len);
    a[3] += *tap * (float)*(buffer + (rd+i*6+3)%len);
    a[4] += *tap * (float)*(buffer + (rd+i*6+4)%len);
    a[5] += *tap * (float)*(buffer + (rd+i*6+5)%len);
    tap++;
  }

  rd=(rd+6*3)%len;
*/

  for (i = 0; i < order_count; i++) {
    j = (rd+i*6)%len;
    k = (rd+ (res_order - 1- i)*6)%len;
    a[0] += *tap * ((float)*(buffer + (j  )) + (float)*(buffer + (k   )));
    a[1] += *tap * ((float)*(buffer + (j+1)) + (float)*(buffer + (k+1 )));
    a[2] += *tap * ((float)*(buffer + (j+2)) + (float)*(buffer + (k+2 )));
    a[3] += *tap * ((float)*(buffer + (j+3)) + (float)*(buffer + (k+3 )));
    a[4] += *tap * ((float)*(buffer + (j+4)) + (float)*(buffer + (k+4 )));
    a[5] += *tap * ((float)*(buffer + (j+5)) + (float)*(buffer + (k+5 )));
    tap++;
  }

  if (order_countN2)
  {
    j = (rd+i*6)%len;
    a[0] += *tap * (float)*(buffer + (j  ));
    a[1] += *tap * (float)*(buffer + (j+1));
    a[2] += *tap * (float)*(buffer + (j+2));
    a[3] += *tap * (float)*(buffer + (j+3));
    a[4] += *tap * (float)*(buffer + (j+4));
    a[5] += *tap * (float)*(buffer + (j+5));
  }

  rd=(rd+6*3)%len;

#ifdef __SATURATION__
  for (i = 0; i < 6; i++)
  {
    if(a[i]>MAX_S16_F)
      a[i]=MAX_S16_F;
    else if(a[i]<MIN_S16_F)
      a[i]=MIN_S16_F;
  }
#endif

  *(outp+offset+0) = (int16_t)a[0];
  *(outp+offset+1) = (int16_t)a[1];
  *(outp+offset+2) = (int16_t)a[2];
  *(outp+offset+3) = (int16_t)a[3];
  *(outp+offset+4) = (int16_t)a[4];
  *(outp+offset+5) = (int16_t)a[5];
}


static void ch4_scale48k(int16_t* outp, int offset)
{
  int i, j, k;
  //int16_t *bufs, *bufe;
  float a[4];
  float* tap = res_coeff;

  for (i = 0; i < 4; i++)
    a[i] = 0;

/* v1
  for (i = 0; i < order; i++) {
    j = (rd+i*4)%len;
    a[0] += *tap * (float)*(buffer + (j  ));
    a[1] += *tap * (float)*(buffer + (j+1));
    a[2] += *tap * (float)*(buffer + (j+2));
    a[3] += *tap * (float)*(buffer + (j+3));
    tap++;
  }
*/

  for (i = 0; i < order_count; i++) {
    j = (rd+i*4)%len;
    k = (rd+ (res_order - 1- i)*4)%len;
    a[0] += *tap * ((float)*(buffer + (j  )) + (float)*(buffer + (k   )));
    a[1] += *tap * ((float)*(buffer + (j+1)) + (float)*(buffer + (k+1 )));
    a[2] += *tap * ((float)*(buffer + (j+2)) + (float)*(buffer + (k+2 )));
    a[3] += *tap * ((float)*(buffer + (j+3)) + (float)*(buffer + (k+3 )));
    tap++;
  }

  if (order_countN2)
  {
    j = (rd+i*4)%len;
    a[0] += *tap * (float)*(buffer + (j  ));
    a[1] += *tap * (float)*(buffer + (j+1));
    a[2] += *tap * (float)*(buffer + (j+2));
    a[3] += *tap * (float)*(buffer + (j+3));
  }

  rd=(rd+4*3)%len;

/* v3
  for (i = 0; i < order_count; i++)
  {
    if ((rd+i*4) >= len)
      bufs = buffer + (rd+i*4)%len;
    else
      bufs = buffer + (rd+i*4);

    if ((rd+ (res_order - 1- i)*4) >= len)
      bufe = buffer + (rd+ (res_order - 1- i)*4)%len;
    else
      bufe = buffer + (rd+ (res_order - 1- i)*4);

    a[0] += *tap * ((float)*(bufs  ) + (float)*(bufe  ));
    a[1] += *tap * ((float)*(bufs+1) + (float)*(bufe+1));
    a[2] += *tap * ((float)*(bufs+2) + (float)*(bufe+2));
    a[3] += *tap * ((float)*(bufs+3) + (float)*(bufe+3));
    tap++;
  }

  if (order_countN2)
  {
    if ((rd+i*4) >= len)
      bufs = buffer + (rd+i*4)%len;
    else
      bufs = buffer + (rd+i*4);

    a[0] += *tap * (float)*(bufs  );
    a[1] += *tap * (float)*(bufs+1);
    a[2] += *tap * (float)*(bufs+2);
    a[3] += *tap * (float)*(bufs+3);
  }

  if ((rd+4*3) >= len)
    rd=(rd+4*3)%len;
  else
    rd=(rd+4*3);
*/


#ifdef __SATURATION__
  for (i = 0; i < 4; i++)
  {
    if(a[i]>MAX_S16_F)
      a[i]=MAX_S16_F;
    else if(a[i]<MIN_S16_F)
      a[i]=MIN_S16_F;
  }
#endif

  *(outp+offset+0) = (int16_t)a[0];
  *(outp+offset+1) = (int16_t)a[1];
  *(outp+offset+2) = (int16_t)a[2];
  *(outp+offset+3) = (int16_t)a[3];
}


static void ch2_scale48k(int16_t* outp, int offset) {
  int i;
  float a[2];
  float* tap = res_coeff;
  for (i = 0; i < 2; i++)
    a[i] = 0;

  for (i = 0; i < res_order; i++) {
    a[0] += *tap * (float)*(buffer + (rd+i*2  )%len);
    a[1] += *tap * (float)*(buffer + (rd+i*2+1)%len);
    tap++;
  }

  rd=(rd+2*3)%len;

#ifdef __SATURATION__
  for (i = 0; i < 2; i++)
  {
    if(a[i]>MAX_S16_F)
      a[i]=MAX_S16_F;
    else if(a[i]<MIN_S16_F)
      a[i]=MIN_S16_F;
  }
#endif

  *(outp+offset+0) = (int16_t)a[0];
  *(outp+offset+1) = (int16_t)a[1];
}

BOOL MApi_VOC_SrcProcess(S16 *ps16Buffer, U32 *pu32Count)
{
  int i = 0;
  int offset =0;
  if(!ps16Buffer || !pu32Count)
    return FALSE;

  count = *pu32Count;
  srcp = ps16Buffer;
  buff = buffer+(res_order-1)*i_ch;
  while(i<count)
  {
    memcpy(buff,(srcp+i),2*i_ch*sizeof(int16_t));//get 2 points every time, order+1 in buffer
    chx_scale(ps16Buffer, buffer, offset, res_order, res_coeff);
    memcpy(buffer, buffer+i_ch*2, (res_order-1)*i_ch*sizeof(int16_t));
    offset+=i_ch;
    i+=i_ch*2;//32k->16k,
  }

  *pu32Count = *pu32Count/2;
  return TRUE;

}

BOOL MApi_VOC_SrcProcess48k(S16 *ps16Buffer, U32 *pu32Count)
{
  int i = 0;
  int offset =0;
  if(!ps16Buffer || !pu32Count)
    return FALSE;

  if (nSrcInitFlag == 0)
  {
    SRC_ERROR("MApi_VOC_SrcProcess48k: init failed\n");
    return FALSE;
  }

  count = *pu32Count;
  srcp = ps16Buffer;

  while(i<count)
  {
    memcpy(buffer+wr,(srcp+i),3*i_ch*sizeof(int16_t));//get 3 points every time, order+3 in buffer
    chx_scale48k(ps16Buffer, offset);
    offset+=i_ch;
    i+=i_ch*3;//48k->16k,
    wr=(wr+i_ch*3)%len;
  }

  *pu32Count = *pu32Count/3;
  return TRUE;

}

BOOL MApi_VOC_SrcInit48k(void)
{
  if(res_order>MAX_FIR_TAB)
  {
    SRC_ERROR("MApi_VOC_SrcInit48k: buffer size is not enough\r\n");
    return FALSE;
  }

  memset(buffer, 0, sizeof(buffer));
  i_ch = MDrv_AUD_GetMicNo();
  wr = (res_order)*i_ch;
  rd = 0;
  len = i_ch*(MAX_FIR_TAB+1);
  order_count = res_order/2;
  order_countN2 = res_order%2;

  switch(i_ch)
  {
    case 2:
      chx_scale48k = ch2_scale48k;
      break;
    case 4:
      chx_scale48k = ch4_scale48k;
      break;
    case 6:
#if defined(CONFIG_AUD_AEC) && defined(CONFIG_AUD_REFEXT)
      if(MDrv_AUD_AECIsEnable())
        chx_scale48k = ch6_scale_wAec48k;
      else
#endif
      chx_scale48k = ch6_scale48k;
      break;
  }

  nSrcInitFlag = 1;

  return TRUE;
}

BOOL MApi_VOC_SrcInit(void)
{
  if(res_order>MAX_FIR_TAB)
  {
    SRC_ERROR("MApi_VOC_SrcInit: buffer size is not enough\r\n");
    return FALSE;
  }

  memset(buffer, 0, sizeof(buffer));
  i_ch = MDrv_AUD_GetMicNo();
  switch(i_ch)
  {
    case 2:
      chx_scale = ch2_scale;
      break;
    case 4:
      chx_scale = ch4_scale;
      break;
    case 6:
#if defined(CONFIG_AUD_AEC) && defined(CONFIG_AUD_REFEXT)
      if(MDrv_AUD_AECIsEnable())
        chx_scale = ch6_scale_wAec;
      else
#endif
      chx_scale = ch6_scale;
      break;
  }
  return TRUE;
}
#endif

