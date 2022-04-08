
#define Color_RootSig\
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), "\
	"RootConstants(b0, num32BitConstants = 4), "\
	"CBV(b1, visibility = SHADER_VISIBILITY_VERTEX), "\
	"CBV(b2, visibility = SHADER_VISIBILITY_PIXEL), "\
	"CBV(b3, visibility = SHADER_VISIBILITY_PIXEL), "\
	"DescriptorTable(SRV(t0, numDescriptors = 1), visibility = SHADER_VISIBILITY_PIXEL), "\
	"DescriptorTable(SRV(t1, numDescriptors = 1), visibility = SHADER_VISIBILITY_PIXEL), "\
	"DescriptorTable(SRV(t2, numDescriptors = 1), visibility = SHADER_VISIBILITY_PIXEL), "\
	"StaticSampler(s0," \
		"addressU = TEXTURE_ADDRESS_WRAP," \
		"addressV = TEXTURE_ADDRESS_WRAP," \
		"addressW = TEXTURE_ADDRESS_WRAP," \
		"filter = FILTER_MIN_MAG_MIP_POINT),"\
	"StaticSampler(s1," \
		"addressU = TEXTURE_ADDRESS_BORDER," \
		"addressV = TEXTURE_ADDRESS_BORDER," \
		"addressW = TEXTURE_ADDRESS_BORDER," \
		"filter = FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,"\
        "mipLODBias =0 ,"\
        "maxAnisotropy = 16,"\
        "comparisonFunc = COMPARISON_LESS_EQUAL,"\
        "borderColor = STATIC_BORDER_COLOR_OPAQUE_BLACK)"



	