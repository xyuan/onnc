#include <onnc/IR/Module.h>
#include <onnc/IR/ONNXUtils.h>
#include <onnx/common/ir_pb_converter.h>

namespace onnc {
namespace onnx {

void ExportModelProto(::onnx::ModelProto &pModelProto, const Module &pModule)
{
  pModelProto.set_ir_version(pModule.m_OnnxIRVersion);
  pModelProto.set_producer_name(pModule.m_OnnxProducerName);
  pModelProto.set_producer_version(pModule.m_OnnxProducerVersion);
  pModelProto.set_domain(pModule.m_OnnxDomain);
  pModelProto.set_model_version(pModule.m_OnnxModelVersion);
  pModelProto.set_doc_string(pModule.m_OnnxDocString);
  ::onnx::ExportModelProto(&pModelProto, pModule.m_OnnxGraph);
  for (auto setId = pModule.m_OnnxSetId.begin();
       setId != pModule.m_OnnxSetId.end(); ++setId) {
    auto *opset_imports = pModelProto.add_opset_import();
    opset_imports->set_domain(setId->key());
    opset_imports->set_version(setId->value());
  }
  for (auto metaData = pModule.m_OnnxMetadata.begin();
       metaData != pModule.m_OnnxMetadata.end(); ++metaData) {
    auto *metadata_props = pModelProto.add_metadata_props();
    metadata_props->set_key(metaData->key());
    metadata_props->set_value(metaData->value());
  }
}

void ImportModelProto(Module &pModule, const ::onnx::ModelProto &pModelProto)
{
  if (pModelProto.has_ir_version()) {
    pModule.m_OnnxIRVersion = pModelProto.ir_version();
  }
  if (pModelProto.has_producer_name()) {
    pModule.m_OnnxProducerName = pModelProto.producer_name();
  }
  if (pModelProto.has_producer_version()) {
    pModule.m_OnnxProducerVersion = pModelProto.producer_version();
  }
  if (pModelProto.has_domain()) {
    pModule.m_OnnxDomain = pModelProto.domain();
  }
  if (pModelProto.has_model_version()) {
    pModule.m_OnnxModelVersion = pModelProto.model_version();
  }
  if (pModelProto.has_doc_string()) {
    pModule.m_OnnxDocString = pModelProto.doc_string();
  }
  pModule.m_OnnxGraph = std::move(::onnx::ImportModelProto(pModelProto));

  pModule.m_OnnxSetId.clear();
  for (int i = 0; i < pModelProto.opset_import_size(); i++) {
    auto &opset = pModelProto.opset_import(i);
    bool retExist;
    pModule.m_OnnxSetId.insert(opset.domain(), retExist)->value() =
        opset.version();
  }
  pModule.m_OnnxMetadata.clear();
  for (int i = 0; i < pModelProto.metadata_props_size(); i++) {
    auto &strStrEntry = pModelProto.metadata_props(i);
    bool retExist;
    pModule.m_OnnxMetadata.insert(strStrEntry.key(), retExist)->value() =
        strStrEntry.value();
  }
}

} // namespace onnx
} // namespace onnc
