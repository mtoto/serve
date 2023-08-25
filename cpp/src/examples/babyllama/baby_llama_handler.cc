#include "src/examples/babyllama/baby_llama_handler.hh"

#include <typeinfo>

namespace llm {

std::pair<std::shared_ptr<torch::jit::script::Module>,
          std::shared_ptr<torch::Device>>
LlmHandler::LoadModel(
    std::shared_ptr<torchserve::LoadModelRequest>& load_model_request) {
  try {
    auto device = GetTorchDevice(load_model_request);
    // Load dummy model
    auto module = std::make_shared<torch::jit::script::Module>(
        torch::jit::load(fmt::format("{}/{}", load_model_request->model_dir,
                                     manifest_->GetModel().serialized_file),
                         *device));

    Transformer transformer;
    char checkpoint_path[] = "/home/ubuntu/serve/cpp/stories15M.bin";
    std::cout << "<<<<<<<<<<<<<<<<<<<<Loading transformers" << std::endl;
    build_transformer(&transformer, checkpoint_path);
    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<Transformers load success"
              << std::endl;

    // std::cout << "Transformer vocab size: " << transformer.config.vocab_size
    //           << std::endl;
    // std::cout << "<<<<<<<<<<<<<<<<<<<<<Loading tokenizer" << std::endl;
    // char tokenizer_path[] = "tokenizer.bin";
    // Tokenizer tokenizer;
    // build_tokenizer(&tokenizer, tokenizer_path,
    // transformer.config.vocab_size);
    // std::cout << "<<<<<<<<<<<<<<<<<<<<<Tokenizer loaded successfully"
    //           << std::endl;

    return std::make_pair(module, device);
  } catch (const c10::Error& e) {
    TS_LOGF(ERROR, "loading the model: {}, device id: {}, error: {}",
            load_model_request->model_name, load_model_request->gpu_id,
            e.msg());
    throw e;
  } catch (const std::runtime_error& e) {
    TS_LOGF(ERROR, "loading the model: {}, device id: {}, error: {}",
            load_model_request->model_name, load_model_request->gpu_id,
            e.what());
    throw e;
  }
}

// std::vector<torch::jit::IValue> LlmHandler::Preprocess(
//     std::shared_ptr<torch::Device>& device,
//     std::pair<std::string&, std::map<uint8_t, std::string>&>& idx_to_req_id,
//     std::shared_ptr<torchserve::InferenceRequestBatch>& request_batch,
//     std::shared_ptr<torchserve::InferenceResponseBatch>& response_batch) {
//   std::cout << "<<<<<<<<<<<<<,Inside custom handler preprocess" << std::endl;

//   initialize_context();

//   std::cout << "Llama context initialized" << std::endl;

// std::vector<torch::jit::IValue> batch_ivalue;
//   std::vector<torch::Tensor> batch_tensors;
//   uint8_t idx = 0;
//   for (auto& request : *request_batch) {
//     try {
//       (*response_batch)[request.request_id] =
//           std::make_shared<torchserve::InferenceResponse>(request.request_id);
//       idx_to_req_id.first += idx_to_req_id.first.empty()
//                                  ? request.request_id
//                                  : "," + request.request_id;

//       auto data_it = request.parameters.find(
//           torchserve::PayloadType::kPARAMETER_NAME_DATA);
//       auto dtype_it =
//           request.headers.find(torchserve::PayloadType::kHEADER_NAME_DATA_TYPE);
//       if (data_it == request.parameters.end()) {
//         data_it = request.parameters.find(
//             torchserve::PayloadType::kPARAMETER_NAME_BODY);
//         dtype_it = request.headers.find(
//             torchserve::PayloadType::kHEADER_NAME_BODY_TYPE);
//       }

//       if (data_it == request.parameters.end() ||
//           dtype_it == request.headers.end()) {
//         TS_LOGF(ERROR, "Empty payload for request id: {}",
//         request.request_id);
//         (*response_batch)[request.request_id]->SetResponse(
//             500, "data_type", torchserve::PayloadType::kCONTENT_TYPE_TEXT,
//             "Empty payload");
//         continue;
//       }

//       std::cout << "Received Input: " << data_it->second << std::endl;

//       // std::vector new_data = request.parameters["data"];
//       // std::string msg = torchserve::Converter::VectorToStr(new_data);
//       std::string msg =
//       torchserve::Converter::VectorToStr(data_it->second);

//       // tokenization

//       std::vector<llama_token> tokens_list;
//       tokens_list = ::llama_tokenize(llama_ctx, msg, true);

//       // const int max_context_size = llama_n_ctx(ctx);
//       const int max_tokens_list_size = max_context_size - 4;

//       if ((int)tokens_list.size() > max_tokens_list_size) {
//         std::cout << __func__ << ": error: prompt too long ("
//                   << tokens_list.size() << " tokens, max "
//                   << max_tokens_list_size << ")\n";
//       }

//       // Print the tokens from the prompt :
//       std::vector<torch::Tensor> tensor_vector;
//       for (auto id : tokens_list) {
//         torch::Tensor tensor = torch::tensor(id, torch::kInt64);
//         tensor_vector.push_back(tensor);
//       }

//       torch::Tensor stacked_tensor = torch::stack(tensor_vector);
//       batch_ivalue.push_back(stacked_tensor);
//       idx_to_req_id.second[idx++] = request.request_id;

//     } catch (const std::runtime_error& e) {
//       TS_LOGF(ERROR, "Failed to load tensor for request id: {}, error: {}",
//               request.request_id, e.what());
//       auto response = (*response_batch)[request.request_id];
//       response->SetResponse(500, "data_type",
//                             torchserve::PayloadType::kDATA_TYPE_STRING,
//                             "runtime_error, failed to load tensor");
//     } catch (const c10::Error& e) {
//       TS_LOGF(ERROR, "Failed to load tensor for request id: {}, c10 error:
//       {}",
//               request.request_id, e.msg());
//       auto response = (*response_batch)[request.request_id];
//       response->SetResponse(500, "data_type",
//                             torchserve::PayloadType::kDATA_TYPE_STRING,
//                             "c10 error, failed to load tensor");
//     }
//   }

// return batch_ivalue;
// }

// torch::Tensor LlmHandler::Inference(
//     std::shared_ptr<torch::jit::script::Module> model,
//     std::vector<torch::jit::IValue>& inputs,
//     std::shared_ptr<torch::Device>& device,
//     std::pair<std::string&, std::map<uint8_t, std::string>&>& idx_to_req_id,
//     std::shared_ptr<torchserve::InferenceResponseBatch>& response_batch) {
//   auto tokens_list_tensor = inputs[0].toTensor();

//   int64_t num_elements = tokens_list_tensor.numel();

//   // Convert the tensor to a vector of long values
//   std::vector<long> long_vector;
//   long_vector.reserve(num_elements);

//   auto data_ptr = tokens_list_tensor.data_ptr<int64_t>();
//   for (int64_t i = 0; i < num_elements; ++i) {
//     long_vector.push_back(data_ptr[i]);
//   }

//   std::vector<llama_token> tokens_list;

//   for (auto id : long_vector) {
//     tokens_list.push_back(id);
//   }
//   const int n_gen = std::min(32, max_context_size);

//   while (llama_get_kv_cache_token_count(llama_ctx) < n_gen) {
//     // evaluate the transformer

//     if (llama_eval(llama_ctx, tokens_list.data(), int(tokens_list.size()),
//                    llama_get_kv_cache_token_count(llama_ctx),
//                    params.n_threads)) {
//       std::cout << "Failed to eval\n" << __func__ << std::endl;
//       break;
//     }

//     tokens_list.clear();

//     // sample the next token

//     llama_token new_token_id = 0;

//     auto logits = llama_get_logits(llama_ctx);
//     auto n_vocab = llama_n_vocab(llama_ctx);

//     std::vector<llama_token_data> candidates;
//     candidates.reserve(n_vocab);

//     for (llama_token token_id = 0; token_id < n_vocab; token_id++) {
//       candidates.emplace_back(
//           llama_token_data{token_id, logits[token_id], 0.0f});
//     }

//     llama_token_data_array candidates_p = {candidates.data(),
//     candidates.size(),
//                                            false};

//     new_token_id = llama_sample_token_greedy(llama_ctx, &candidates_p);

//     // is it an end of stream ?
//     if (new_token_id == llama_token_eos(llama_ctx)) {
//       std::cout << "Reached [end of text]\n";
//       break;
//     }

//     // print the new token :
//     std::cout << "New Token: " << llama_token_to_str(llama_ctx, new_token_id)
//               << std::endl;

//     // push this new token for next evaluation
//     tokens_list.push_back(new_token_id);
//   }

//   std::vector<torch::Tensor> tensor_vector;
//   for (auto id : tokens_list) {
//     torch::Tensor tensor = torch::tensor(id, torch::kLong);
//     tensor_vector.push_back(tensor);
//   }

//   torch::Tensor stacked_tensor = torch::stack(tensor_vector);
//   llama_print_timings(llama_ctx);
//   llama_free(llama_ctx);
//   return stacked_tensor;
// }

// void LlmHandler::Postprocess(
//     const torch::Tensor& data,
//     std::pair<std::string&, std::map<uint8_t, std::string>&>& idx_to_req_id,
//     std::shared_ptr<torchserve::InferenceResponseBatch>& response_batch) {
//   for (const auto& kv : idx_to_req_id.second) {
//     try {
//       int64_t num_elements = data.numel();

//       // Convert the tensor to a vector of long values
//       std::stringstream generated_text_stream;

//       auto data_ptr = data.data_ptr<int64_t>();
//       for (int64_t i = 0; i < num_elements; ++i) {
//         generated_text_stream << llama_token_to_str(llama_ctx, data_ptr[i]);
//       }

//       std::string generated_text_str = generated_text_stream.str();
//       std::cout << "Generated Text Str: " << generated_text_str << std::endl;

//       auto response = (*response_batch)[kv.second];

//       response->SetResponse(200, "data_type",
//                             torchserve::PayloadType::kDATA_TYPE_STRING,
//                             generated_text_str);
//     } catch (const std::runtime_error& e) {
//       TS_LOGF(ERROR, "Failed to load tensor for request id: {}, error: {}",
//               kv.second, e.what());
//       auto response = (*response_batch)[kv.second];
//       response->SetResponse(500, "data_type",
//                             torchserve::PayloadType::kDATA_TYPE_STRING,
//                             "runtime_error, failed to postprocess tensor");
//     } catch (const c10::Error& e) {
//       TS_LOGF(ERROR,
//               "Failed to postprocess tensor for request id: {}, error: {}",
//               kv.second, e.msg());
//       auto response = (*response_batch)[kv.second];
//       response->SetResponse(500, "data_type",
//                             torchserve::PayloadType::kDATA_TYPE_STRING,
//                             "c10 error, failed to postprocess tensor");
//     }
//   }
// }

}  // namespace llm

#if defined(__linux__) || defined(__APPLE__)
extern "C" {
torchserve::torchscripted::BaseHandler* allocatorLlmHandler() {
  return new llm::LlmHandler();
}

void deleterLlmHandler(torchserve::torchscripted::BaseHandler* p) {
  if (p != nullptr) {
    delete static_cast<llm::LlmHandler*>(p);
  }
}
}
#endif
