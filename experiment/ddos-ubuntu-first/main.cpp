#include <iostream>
#include <net.h>

void pretty_print(const ncnn::Mat& m)
{
	for (int q = 0; q < m.c; q++)
	{
		const float* ptr = m.channel(q);
		for (int y = 0; y < m.h; y++)
		{
			for (int x = 0; x < m.w; x++)
			{
				printf("%f ", ptr[x]);
			}
			ptr += m.w;
			printf("\n");
		}
		printf("------------------------\n");
	}
}

int main(int argc, char* argv[]) {
	ncnn::Net* net = new ncnn::Net();
	net->opt.use_vulkan_compute = false;
	net->opt.use_fp16_arithmetic = true;
	//net->load_param("D:/github/ncnn-resources/model.pnnx.param");
	//net->load_model("D:/github/ncnn-resources/model.pnnx.bin");

	if(argc == 2) {
		std::string path(argv[1]);
		std::string paramPath = path + ".param";
		std::string modelPath = path + ".bin";
		net->load_param(paramPath.c_str());
		net->load_model(modelPath.c_str());
	}else{
		net->load_param("../../../resource/mlddos/mlddos.ncnn.param");
		net->load_model("../../../resource/mlddos/mlddos.ncnn.bin");
	}

	auto ex = net->create_extractor();
	ex.set_light_mode(false);
	ex.set_num_threads(1);

	ncnn::Mat input;

	float inputData[63] = { 0.4963f, 0.7682f, 0.0885, 0.1320, 0.3074, 0.6341, 0.4901, 0.8964, 0.4556,
		0.6323, 0.3489, 0.4017, 0.0223, 0.1689, 0.2939, 0.5185, 0.6977, 0.8000,
		0.1610, 0.2823, 0.6816, 0.9152, 0.3971, 0.8742, 0.4194, 0.5529, 0.9527,
		0.0362, 0.1852, 0.3734, 0.3051, 0.9320, 0.1759, 0.2698, 0.1507, 0.0317,
		0.2081, 0.9298, 0.7231, 0.7423, 0.5263, 0.2437, 0.5846, 0.0332, 0.1387,
		0.2422, 0.8155, 0.7932, 0.2783, 0.4820, 0.8198, 0.9971, 0.6984, 0.5675,
		0.8352, 0.2056, 0.5932, 0.1123, 0.1535, 0.2417, 0.7262, 0.7011, 0.2038 };

	uint16_t coveredData[63] = { 0 };

	for (int i = 0; i < 63; ++i) {
		float f = inputData[i];
		uint32_t x = *((uint32_t*)&f);
		uint16_t h = ((x >> 16) & 0x8000) | ((((x & 0x7f800000) - 0x38000000) >> 13) & 0x7c00) | ((x >> 13) & 0x03ff);
		coveredData[i] = h;
	}

	input = ncnn::Mat::from_float16((const unsigned short *)coveredData, 63);
	
	pretty_print(input);

	ncnn::Mat out;

	for(int i = 0; i < 100; ++i) {
		ex.input("in0", input);
		//ex.extract("F.log_softmax_0", out);
		ex.extract("out0", out);
	}



	pretty_print(out);

	return 0;
}