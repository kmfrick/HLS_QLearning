/*------------------------------------------------------------------------*
 * This file contains routines written by Rich Sutton and Chuck Anderson. *
 * Claude Sammut translated parts from Fortran to C.					  *
 * Kevin Michael Frick and Davide Ragazzini adapted these routines to	  *
 * run on FPGA hardware.												  *
 * ---------------------------------------------------------------------- */

#include "pole.hpp"
#include "mtwister.hpp"

#include <hls_math.h>
#include <hls_stream.h>

#define random_01 ((float)(hls_rand_stream.read()  / (float)((1 << 31) - 1)))
#define random_action (hls_rand_stream.read() % N_ACTIONS)

#define MOV_AVG_INTERVAL 50

// Alpha: Q-table learning rate
// alpha = max(ALPHA_MIN, min(1.0f, 1.0f - log10((failures + 1.0f) / 25.0f)))
// ALPHA_MIN = 0.1f
#define ALPHA(failures)		ALPHA_ARR[failures]
const float ALPHA_ARR[MAX_FAILURES] = { 1.000000, 1.000000, 1.000000, 1.000000,
		1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		0.982967, 0.966576, 0.950782, 0.935542, 0.920819, 0.906578, 0.892790,
		0.879426, 0.866461, 0.853872, 0.841638, 0.829738, 0.818156, 0.806875,
		0.795880, 0.785156, 0.774691, 0.764472, 0.754487, 0.744727, 0.735182,
		0.725842, 0.716699, 0.707744, 0.698970, 0.690370, 0.681937, 0.673664,
		0.665546, 0.657577, 0.649752, 0.642065, 0.634512, 0.627088, 0.619789,
		0.612610, 0.605548, 0.598599, 0.591760, 0.585027, 0.578396, 0.571865,
		0.565431, 0.559091, 0.552842, 0.546682, 0.540608, 0.534617, 0.528708,
		0.522879, 0.517126, 0.511449, 0.505845, 0.500313, 0.494850, 0.489455,
		0.484126, 0.478862, 0.473661, 0.468521, 0.463442, 0.458421, 0.453457,
		0.448550, 0.443697, 0.438899, 0.434152, 0.429457, 0.424812, 0.420216,
		0.415669, 0.411168, 0.406714, 0.402305, 0.397940, 0.393619, 0.389340,
		0.385103, 0.380907, 0.376751, 0.372634, 0.368556, 0.364516, 0.360514,
		0.356547, 0.352617, 0.348722, 0.344862, 0.341035, 0.337242, 0.333482,
		0.329754, 0.326058, 0.322393, 0.318759, 0.315155, 0.311580, 0.308035,
		0.304518, 0.301030, 0.297569, 0.294136, 0.290730, 0.287350, 0.283997,
		0.280669, 0.277366, 0.274088, 0.270835, 0.267606, 0.264401, 0.261219,
		0.258061, 0.254925, 0.251812, 0.248721, 0.245652, 0.242604, 0.239578,
		0.236572, 0.233587, 0.230623, 0.227678, 0.224754, 0.221849, 0.218963,
		0.216096, 0.213249, 0.210419, 0.207608, 0.204815, 0.202040, 0.199283,
		0.196543, 0.193820, 0.191114, 0.188425, 0.185752, 0.183096, 0.180456,
		0.177832, 0.175224, 0.172631, 0.170053, 0.167491, 0.164944, 0.162412,
		0.159894, 0.157391, 0.154902, 0.152427, 0.149967, 0.147520, 0.145087,
		0.142668, 0.140261, 0.137869, 0.135489, 0.133122, 0.130768, 0.128427,
		0.126098, 0.123782, 0.121478, 0.119186, 0.116907, 0.114639, 0.112383,
		0.110138, 0.107905, 0.105684, 0.103474, 0.101275, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000 };/* 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000, 0.100000,
		0.100000, 0.100000, 0.100000, 0.100000, 0.100000 };*/

// Epsilon: experimentation threshold
// eps = EPS_END + (EPS_START - EPS_END) * exp(-1.0f * (failures) / EPS_DECAY)
// EPS_START	0.9f
// EPS_END		0.05f
// EPS_DECAY	200.0f
#define EPS(failures) EPS_ARR[failures]
const float EPS_ARR[MAX_FAILURES] = { 0.900000, 0.895761, 0.891542, 0.887345,
		0.883169, 0.879013, 0.874879, 0.870765, 0.866671, 0.862598, 0.858545,
		0.854512, 0.850500, 0.846507, 0.842535, 0.838582, 0.834649, 0.830735,
		0.826842, 0.822967, 0.819112, 0.815276, 0.811459, 0.807661, 0.803882,
		0.800122, 0.796381, 0.792659, 0.788955, 0.785269, 0.781602, 0.777953,
		0.774322, 0.770710, 0.767115, 0.763538, 0.759980, 0.756439, 0.752915,
		0.749409, 0.745921, 0.742450, 0.738997, 0.735560, 0.732141, 0.728739,
		0.725354, 0.721985, 0.718634, 0.715299, 0.711981, 0.708679, 0.705394,
		0.702125, 0.698873, 0.695636, 0.692416, 0.689212, 0.686024, 0.682852,
		0.679695, 0.676555, 0.673430, 0.670321, 0.667227, 0.664148, 0.661085,
		0.658037, 0.655005, 0.651987, 0.648985, 0.645997, 0.643025, 0.640067,
		0.637124, 0.634196, 0.631282, 0.628383, 0.625498, 0.622628, 0.619772,
		0.616930, 0.614103, 0.611289, 0.608490, 0.605704, 0.602933, 0.600175,
		0.597431, 0.594701, 0.591984, 0.589281, 0.586591, 0.583915, 0.581252,
		0.578602, 0.575966, 0.573343, 0.570732, 0.568135, 0.565551, 0.562980,
		0.560421, 0.557876, 0.555342, 0.552822, 0.550314, 0.547819, 0.545336,
		0.542866, 0.540407, 0.537961, 0.535528, 0.533106, 0.530697, 0.528299,
		0.525914, 0.523540, 0.521178, 0.518828, 0.516490, 0.514163, 0.511848,
		0.509545, 0.507253, 0.504972, 0.502703, 0.500445, 0.498199, 0.495963,
		0.493739, 0.491526, 0.489324, 0.487132, 0.484952, 0.482783, 0.480624,
		0.478477, 0.476340, 0.474213, 0.472098, 0.469992, 0.467898, 0.465813,
		0.463739, 0.461676, 0.459623, 0.457580, 0.455547, 0.453524, 0.451512,
		0.449509, 0.447516, 0.445534, 0.443561, 0.441598, 0.439645, 0.437702,
		0.435768, 0.433844, 0.431930, 0.430025, 0.428129, 0.426243, 0.424367,
		0.422500, 0.420642, 0.418793, 0.416954, 0.415124, 0.413303, 0.411491,
		0.409688, 0.407894, 0.406109, 0.404333, 0.402565, 0.400807, 0.399057,
		0.397316, 0.395584, 0.393861, 0.392146, 0.390439, 0.388741, 0.387052,
		0.385371, 0.383698, 0.382034, 0.380378, 0.378730, 0.377090, 0.375459,
		0.373836, 0.372221, 0.370614, 0.369014, 0.367423, 0.365840, 0.364265,
		0.362698, 0.361138, 0.359586, 0.358042, 0.356506, 0.354977, 0.353456,
		0.351942, 0.350436, 0.348938, 0.347447, 0.345964, 0.344487, 0.343019,
		0.341557, 0.340103, 0.338656, 0.337217, 0.335784, 0.334359, 0.332940,
		0.331529, 0.330125, 0.328728, 0.327338, 0.325955, 0.324578, 0.323209,
		0.321846, 0.320490, 0.319141, 0.317799, 0.316463, 0.315134, 0.313812,
		0.312496, 0.311187, 0.309884, 0.308588, 0.307298, 0.306015, 0.304738,
		0.303468, 0.302204, 0.300946, 0.299694, 0.298449, 0.297210, 0.295977,
		0.294750, 0.293529, 0.292314, 0.291106, 0.289903, 0.288707, 0.287516,
		0.286332, 0.285153, 0.283980, 0.282813, 0.281652, 0.280497, 0.279347,
		0.278203, 0.277065, 0.275933, 0.274806, 0.273684, 0.272569, 0.271459,
		0.270354, 0.269255, 0.268162, 0.267074, 0.265991, 0.264914, 0.263842,
		0.262775, 0.261714, 0.260658, 0.259607, 0.258562, 0.257522, 0.256487,
		0.255457, 0.254432, 0.253413, 0.252398, 0.251389, 0.250384, 0.249385,
		0.248390, 0.247401, 0.246416, 0.245437, 0.244462, 0.243492, 0.242527,
		0.241567, 0.240611, 0.239661, 0.238715, 0.237773, 0.236837, 0.235905,
		0.234978, 0.234055, 0.233137, 0.232224, 0.231315, 0.230411, 0.229511,
		0.228616, 0.227725, 0.226838, 0.225956, 0.225079, 0.224206, 0.223337,
		0.222472, 0.221612, 0.220756, 0.219904, 0.219057, 0.218214, 0.217375,
		0.216540, 0.215710, 0.214883, 0.214061, 0.213242, 0.212428, 0.211618,
		0.210812, 0.210010, 0.209212, 0.208418, 0.207628, 0.206842, 0.206059,
		0.205281, 0.204507, 0.203736, 0.202969, 0.202206, 0.201447, 0.200692,
		0.199940, 0.199192, 0.198448, 0.197708, 0.196971, 0.196238, 0.195509,
		0.194783, 0.194061, 0.193342, 0.192628, 0.191916, 0.191208, 0.190504,
		0.189803, 0.189106, 0.188412, 0.187722, 0.187035, 0.186352, 0.185671,
		0.184995, 0.184322, 0.183652, 0.182985, 0.182322, 0.181662, 0.181005,
		0.180352, 0.179702, 0.179055, 0.178411, 0.177771, 0.177133, 0.176499,
		0.175868, 0.175241, 0.174616, 0.173994, 0.173376, 0.172761, 0.172148,
		0.171539, 0.170933, 0.170330, 0.169730, 0.169133, 0.168538, 0.167947,
		0.167359, 0.166774, 0.166191, 0.165612};/* 0.165035, 0.164461, 0.163890,
		0.163322, 0.162757, 0.162195, 0.161635, 0.161078, 0.160524, 0.159973,
		0.159425, 0.158879, 0.158336, 0.157796, 0.157258, 0.156723, 0.156191,
		0.155661, 0.155134, 0.154610, 0.154088, 0.153569, 0.153052, 0.152538,
		0.152027, 0.151518, 0.151012, 0.150508, 0.150007, 0.149508, 0.149012,
		0.148518, 0.148026, 0.147537, 0.147051, 0.146567, 0.146085, 0.145606,
		0.145129, 0.144655, 0.144183, 0.143713, 0.143246, 0.142780, 0.142318,
		0.141857, 0.141399, 0.140943, 0.140490, 0.140038, 0.139589, 0.139143,
		0.138698, 0.138256, 0.137815, 0.137377, 0.136942, 0.136508, 0.136076,
		0.135647, 0.135220, 0.134795, 0.134372, 0.133951, 0.133533, 0.133116,
		0.132701, 0.132289, 0.131878, 0.131470, 0.131064, 0.130659, 0.130257,
		0.129857, 0.129459, 0.129062, 0.128668, 0.128276, 0.127885, 0.127497,
		0.127110, 0.126726, 0.126343, 0.125962, 0.125583, 0.125206, 0.124831,
		0.124458, 0.124087, 0.123717, 0.123350, 0.122984, 0.122620, 0.122258,
		0.121897, 0.121539, 0.121182, 0.120827, 0.120473, 0.120122, 0.119772,
		0.119424, 0.119078, 0.118733, 0.118391, 0.118050, 0.117710, 0.117372,
		0.117036, 0.116702, 0.116369, 0.116038, 0.115709, 0.115381, 0.115055,
		0.114731, 0.114408, 0.114087, 0.113767, 0.113449, 0.113133, 0.112818,
		0.112504, 0.112193, 0.111882, 0.111574, 0.111267, 0.110961, 0.110657,
		0.110355, 0.110054, 0.109754, 0.109456, 0.109159, 0.108864, 0.108571,
		0.108279, 0.107988, 0.107699, 0.107411, 0.107125, 0.106840, 0.106556,
		0.106274, 0.105994, 0.105714, 0.105436, 0.105160, 0.104885, 0.104611,
		0.104339, 0.104068, 0.103798, 0.103530, 0.103263, 0.102997, 0.102733,
		0.102470, 0.102208, 0.101948, 0.101689, 0.101431, 0.101174, 0.100919,
		0.100665, 0.100412, 0.100161, 0.099911, 0.099662, 0.099414, 0.099168,
		0.098922, 0.098678, 0.098436, 0.098194, 0.097954, 0.097715, 0.097477,
		0.097240, 0.097004, 0.096770, 0.096536, 0.096304, 0.096073, 0.095844,
		0.095615, 0.095387, 0.095161, 0.094936, 0.094712, 0.094489, 0.094267,
		0.094046, 0.093826, 0.093608, 0.093390, 0.093174, 0.092959, 0.092744,
		0.092531, 0.092319, 0.092108, 0.091898, 0.091689, 0.091481, 0.091274,
		0.091068, 0.090863, 0.090660, 0.090457, 0.090255, 0.090054, 0.089855,
		0.089656, 0.089458, 0.089261, 0.089065, 0.088871, 0.088677, 0.088484,
		0.088292, 0.088101, 0.087911, 0.087722, 0.087534, 0.087346, 0.087160,
		0.086975, 0.086790, 0.086607, 0.086424, 0.086243, 0.086062, 0.085882,
		0.085703, 0.085525, 0.085348, 0.085172, 0.084996, 0.084822, 0.084648,
		0.084475, 0.084303, 0.084132, 0.083962, 0.083792, 0.083624, 0.083456,
		0.083289, 0.083123, 0.082958, 0.082794, 0.082630, 0.082467, 0.082305,
		0.082144, 0.081984, 0.081824, 0.081666, 0.081508, 0.081351, 0.081194,
		0.081039, 0.080884, 0.080730, 0.080577, 0.080424, 0.080272, 0.080121,
		0.079971, 0.079822, 0.079673, 0.079525, 0.079378, 0.079231, 0.079085,
		0.078940, 0.078796, 0.078652, 0.078509, 0.078367, 0.078226, 0.078085,
		0.077945, 0.077806, 0.077667, 0.077529, 0.077392, 0.077255, 0.077119,
		0.076984, 0.076849, 0.076715, 0.076582, 0.076449, 0.076318, 0.076186,
		0.076056, 0.075926, 0.075796, 0.075668, 0.075540, 0.075412, 0.075286,
		0.075160, 0.075034, 0.074909, 0.074785, 0.074661, 0.074538, 0.074416,
		0.074294, 0.074173, 0.074052, 0.073932, 0.073813, 0.073694, 0.073576,
		0.073459, 0.073342, 0.073225, 0.073109, 0.072994, 0.072879, 0.072765,
		0.072652, 0.072539, 0.072426, 0.072314, 0.072203, 0.072092, 0.071982,
		0.071873, 0.071764, 0.071655, 0.071547, 0.071440, 0.071333, 0.071226,
		0.071120, 0.071015, 0.070910, 0.070806, 0.070702, 0.070599, 0.070496,
		0.070394, 0.070292, 0.070191, 0.070090, 0.069990, 0.069890, 0.069791,
		0.069692, 0.069594, 0.069497, 0.069399, 0.069303, 0.069206, 0.069110,
		0.069015, 0.068920, 0.068826, 0.068732, 0.068639, 0.068546, 0.068453,
		0.068361, 0.068270, 0.068178, 0.068088, 0.067998, 0.067908, 0.067818,
		0.067730, 0.067641, 0.067553, 0.067466, 0.067379, 0.067292, 0.067206,
		0.067120, 0.067034, 0.066949, 0.066865, 0.066781, 0.066697, 0.066614,
		0.066531, 0.066449, 0.066366, 0.066285, 0.066204, 0.066123, 0.066042,
		0.065962, 0.065883, 0.065804, 0.065725, 0.065646 };*/

short learn(volatile int *rng_state, volatile twobits *running, volatile qtable q_shared[N_AGENTS], volatile short failures[N_AGENTS], ap_uint<8> id) {
#pragma HLS INTERFACE s_axilite port=return
#pragma HLS INTERFACE ap_none port=id
#pragma HLS INTERFACE ap_none port=running
#pragma HLS INTERFACE s_axilite port=q_shared
#pragma HLS INTERFACE s_axilite port=rng_state
#pragma HLS INTERFACE s_axilite port=failures
	*running = 1;
	float p, oldp, rhat, r;
	qvalue q_max;		// used to argmax over actions
	qvalue q_tilde;
	int i, j, k;
	int failed;
	int action;
	int new_state;
	int state;
	float x; 			// cart position, meters
	float x_dot;		// cart velocity
	float theta;		// pole angle, radians
	float theta_dot;	// pole angular velocity
	float xacc, thetaacc, force, costheta, sintheta, temp;
	float moving_avg = -1;
	int cur_steps;
	int steps_sum = 0;
	int steps[MAX_FAILURES];
	// Initialize RNG
	ap_uint<32> seed = *rng_state;
	hls::stream<ap_uint<32> > hls_rand_stream;
#pragma HLS STREAM variable=hls_rand_stream depth=4096 dim=1
	ap_uint<32> stream_length = MAX_FAILURES * OBJECTIVE;
	mtwist_core(true, seed, stream_length, hls_rand_stream);
	/*// Initialize Q-table
	for (i = 0; i < N_BOXES; i++) {
		for (j = 0; j < N_ACTIONS; j++) {
			q_shared[id][i][j] = 0.8;//random_01;
		}
	}*/
	// Starting state is (0 0 0 0)
	x = x_dot = theta = theta_dot = 0.0f;


	// Find box in state space containing start state
	new_state = discretize(x, x_dot, theta, theta_dot);

	cur_steps = 0;
	failures[id] = 0;
	// Iterate through the action-learn loop.
	while (cur_steps < MAX_STEPS && failures[id] < MAX_FAILURES) {
		// Experiment with probability EPS
		if (random_01 <= EPS(failures[id])) {
			action = random_action;
		} else {
			// Begin action argmax
			action = 0;
			q_max = q_shared[id][new_state][0];
			for (i = 0; i < N_ACTIONS; i++) {
				q_tilde = 0;
				for (k = 0; k < N_AGENTS; k++) {
#pragma HLS UNROLL
					q_tilde += q_shared[k][new_state][i] * failures[k];
				}
				q_tilde /= N_AGENTS;
				if (q_tilde > q_max) {
					q_max = q_tilde;
					action = i;
				}
			}
			// End action argmax
		}

		// Begin cart-pole simulation
		// Takes an action (0 or 1) and the current values of the
		// four state variables and updates their values by estimating the state
		// TAU seconds later.

		force = (action > 0) ? FORCE_MAG : -FORCE_MAG;
		sintheta = hls::sin(theta);
		costheta = hls::cos(theta);


		temp = (force + POLEMASS_LENGTH * theta_dot * theta_dot * sintheta)
				/ TOTAL_MASS;

		thetaacc = (GRAVITY * sintheta - costheta * temp)
				/ (LENGTH
						* (FOURTHIRDS
								- MASSPOLE * costheta * costheta / TOTAL_MASS));

		xacc = temp - POLEMASS_LENGTH * thetaacc * costheta / TOTAL_MASS;

		// Update the four state variables, using Euler's method.

		x += TAU * x_dot;
		x_dot += TAU * xacc;
		theta += TAU * theta_dot;
		theta_dot += TAU * thetaacc;
		// End cart-pole simulation

		// Get box of state space containing the resulting state.
		state = new_state;
		new_state = discretize(x, x_dot, theta, theta_dot);
		if (new_state < 0) {
			// Failure occurred
			steps[failures[id]] = cur_steps;
			failed = 1;
			failures[id]++;
			steps_sum += cur_steps;
			if (failures[id] >= MOV_AVG_INTERVAL) {
				moving_avg = steps_sum / float(MOV_AVG_INTERVAL);
				steps_sum -= steps[failures[id] - MOV_AVG_INTERVAL];
			}
#ifndef __SYNTHESIS__
			printf("Trial %d was %d steps.\n", failures[id], cur_steps);
			printf("EPS = %.3f, ALPHA = %.3f\n", EPS(failures[id]),
					ALPHA(failures[id]));
			if (failures[id] >= MOV_AVG_INTERVAL) {
				printf("Moving average of %d steps = %.2f\n", MOV_AVG_INTERVAL, moving_avg);
			}
#endif
			// Stop learning if the objective is reached

			if (moving_avg > OBJECTIVE) {
				*running = 0;
#ifndef __SYNTHESIS__
				printf("{ ");
				for (i = 0; i < N_BOXES; i++) {
					for (j = 0; j < N_ACTIONS; j++) {
						if (j == 0) {
							printf("{ ");
						}
						printf("%.3f ", q_shared[id][i][j]);
						if (j < N_ACTIONS - 1) {
							printf(", ");
						} else {
							printf("},\n");
						}
					}

				}
				printf("};\n");
#endif
				return failures[id];
			}
			// Reset state to (0 0 0 0).  Find the box.
			x = x_dot = theta = theta_dot = 0.0f;
			new_state = discretize(x, x_dot, theta, theta_dot);
			cur_steps = 0;

			// Begin argmax
			q_max = q_shared[id][new_state][0];
			for (i = 0; i < N_ACTIONS; i++) {
				if (q_shared[id][new_state][i] > q_max) {
					q_max = q_shared[id][new_state][i];
				}
			}
			// End argmax
			// Reinforcement upon failure is -1
			q_shared[id][state][action] += ALPHA(failures[id])* (-1 + GAMMA * q_max - q_shared[id][state][action]);

		} else {
			// Not a failure.
			failed = 0;
			// Begin argmax
			q_max = q_shared[id][new_state][0];
			for (i = 0; i < N_ACTIONS; i++) {
				if (q_shared[id][new_state][i] > q_max) {
					q_max = q_shared[id][new_state][i];
				}
			}
			// End argmax
			// Reinforcement is 0
			q_shared[id][state][action] += ALPHA(failures[id]) * (0 + GAMMA * q_max - q_shared[id][state][action]);
			cur_steps++;
		}
	}
#ifndef __SYNTHESIS__
	printf("{ ");
	for (i = 0; i < N_BOXES; i++) {
		for (j = 0; j < N_ACTIONS; j++) {
			if (j == 0) {
				printf("{ ");
			}
			printf("%.3f ", q_shared[id][i][j]);
			if (j < N_ACTIONS - 1) {
				printf(", ");
			} else {
				printf("},\n");
			}
		}

	}
	printf("};\n");
#endif
	*running = 11;
	return failures[id];
}

int discretize(float x, float x_dot, float theta, float theta_dot) {
	int box = 0;

	if (x < -X_BOUND || x > X_BOUND || theta < -twelve_degrees
			|| theta > twelve_degrees)
		return (-1); // to signal failure

	if (x < -0.8f)
		box = 0;
	else if (x < 0.8f)
		box = 1;
	else
		box = 2;

	if (x_dot < -0.5f)
		// noop
		;
	else if (x_dot < 0.5f)
		box += 3;
	else
		box += 6;

	if (theta < -six_degrees)
		// noop
		;
	else if (theta < -one_degree)
		box += 9;
	else if (theta < 0)
		box += 18;
	else if (theta < one_degree)
		box += 27;
	else if (theta < six_degrees)
		box += 36;
	else
		box += 45;

	if (theta_dot < -fifty_degrees)
		;
	else if (theta_dot < fifty_degrees)
		box += 54;
	else
		box += 108;

	return (box);
}
