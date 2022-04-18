
#define Shadow_RootSig\
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), "\
	"RootConstants(b0, num32BitConstants = 4), "\
	"CBV(b1, visibility = SHADER_VISIBILITY_ALL), "\
	"DescriptorTable(SRV(t0, numDescriptors = 1),visibility = SHADER_VISIBILITY_ALL), "\
	"DescriptorTable(SRV(t1, numDescriptors = 1),visibility = SHADER_VISIBILITY_ALL), "\
	"StaticSampler(s0, "\
		"addressU = TEXTURE_ADDRESS_CLAMP,"\
		"addressV = TEXTURE_ADDRESS_CLAMP,"\
		"addressW = TEXTURE_ADDRESS_CLAMP,"\
		"filter = FILTER_MIN_MAG_MIP_LINEAR),"