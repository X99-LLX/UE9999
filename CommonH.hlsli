
#define Test_RootSig\
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), "\
	"RootConstants(b0, num32BitConstants = 4), "\
	"CBV(b1, visibility = SHADER_VISIBILITY_VERTEX), "\
	"DescriptorTable(SRV(t0, numDescriptors = 2), visibility = SHADER_VISIBILITY_PIXEL), "\
	"DescriptorTable(SRV(t2, numDescriptors = 1), visibility = SHADER_VISIBILITY_PIXEL), "\
	"StaticSampler(s0, "\
		"addressU = TEXTURE_ADDRESS_CLAMP,"\
		"addressV = TEXTURE_ADDRESS_CLAMP,"\
		"addressW = TEXTURE_ADDRESS_CLAMP,"\
		"filter = FILTER_MIN_MAG_MIP_LINEAR),"