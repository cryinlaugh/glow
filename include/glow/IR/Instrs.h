#ifndef GLOW_IR_INSTRS_H
#define GLOW_IR_INSTRS_H

#include "glow/IR/IR.h"
#include "glow/IR/Type.h"

namespace glow {

class CopyInst : public Instruction {
public:
  CopyInst(Value *dest, Value *src)
      : Instruction({{dest, OperandKind::kOut}, {src, OperandKind::kIn}}) {}
  StringRef getValueName() override { return "copy"; }
};

class ConvolutionInst : public Instruction {
  size_t kernel_;
  size_t stride_;
  size_t pad_;
  size_t depth_;

public:
  ConvolutionInst(Value *dest, Value *src, Value *filter, Value *bias,
                  size_t kernel, size_t stride, size_t pad, size_t depth)
      : Instruction({{dest, OperandKind::kOut},
                     {src, OperandKind::kIn},
                     {filter, OperandKind::kIn},
                     {bias, OperandKind::kIn}}),

        kernel_(kernel), stride_(stride), pad_(pad), depth_(depth) {}

  StringRef getValueName() override { return "convolution"; }

  std::string getExtraDesc() override;
};

class PoolInst : public Instruction {
public:
  /// Specifies the kind of pooling done by the operator.
  enum class OpKind {
    kMax,
    kAvg,
  };

private:
  size_t kernel_;
  size_t stride_;
  size_t pad_;
  OpKind kind_;

  const char *getKindStr();

public:
  PoolInst(Value *dest, Value *src, Value *srcXY, OpKind kind, size_t kernel,
           size_t stride, size_t pad)
      : Instruction({{dest, OperandKind::kOut},
                     {src, OperandKind::kIn},
                     {srcXY, OperandKind::kInOut}}),
        kernel_(kernel), stride_(stride), pad_(pad), kind_(kind) {}

  StringRef getValueName() override { return "pool"; }
  std::string getExtraDesc() override;
};

class FullyConnectedInst : public Instruction {
  size_t depth_;

public:
  FullyConnectedInst(Value *dest, Value *src, Value *filter, Value *bias,
                     size_t depth)
      : Instruction({{dest, OperandKind::kOut},
                     {src, OperandKind::kIn},
                     {filter, OperandKind::kIn},
                     {bias, OperandKind::kIn}}),
        depth_(depth) {}

  StringRef getValueName() override { return "fullyconnected"; }
  std::string getExtraDesc() override;
};

class ReluInst : public Instruction {
public:
  ReluInst(Value *dest, Value *src)
      : Instruction({{dest, OperandKind::kOut}, {src, OperandKind::kIn}}) {}
  StringRef getValueName() override { return "relu"; }
};

class SigmoidInst : public Instruction {
public:
  SigmoidInst(Value *dest, Value *src)
      : Instruction({{dest, OperandKind::kOut}, {src, OperandKind::kIn}}) {}
  StringRef getValueName() override { return "sigmoid"; }
};

class TanhInst : public Instruction {
public:
  TanhInst(Value *dest, Value *src)
      : Instruction({{dest, OperandKind::kOut}, {src, OperandKind::kIn}}) {}
  StringRef getValueName() override { return "tanh"; }
};

class SoftMaxInst : public Instruction {
public:
  SoftMaxInst(Value *dest, Value *src, Value *expected)
      : Instruction({{dest, OperandKind::kOut},
                     {src, OperandKind::kIn},
                     {expected, OperandKind::kIn}}) {}
  StringRef getValueName() override { return "softmax"; }
};

class RegressionInst : public Instruction {
public:
  RegressionInst(Value *dest, Value *src, Value *expected)
      : Instruction({{dest, OperandKind::kOut},
                     {src, OperandKind::kIn},
                     {expected, OperandKind::kIn}}) {}
  StringRef getValueName() override { return "regression"; }
};

class TransposeInst : public Instruction {
  std::vector<unsigned> shuffle_;

public:
  TransposeInst(Value *dest, Value *src, ArrayRef<unsigned> shuffle)
      : Instruction({{dest, OperandKind::kOut}, {src, OperandKind::kIn}}),
        shuffle_(shuffle.begin(), shuffle.end()) {}
  StringRef getValueName() override { return "transpose"; }

  std::string getExtraDesc() override;
};

class ReshapeInst : public Instruction {
  std::vector<size_t> dims_;

public:
  ReshapeInst(Value *dest, Value *src, ArrayRef<size_t> dims)
      : Instruction({{dest, OperandKind::kOut}, {src, OperandKind::kIn}}),
        dims_(dims.begin(), dims.end()) {}
  StringRef getValueName() override { return "reshape"; }

  std::string getExtraDesc() override;
};

class ConcatInst : public Instruction {
  size_t dim_;

public:
  ConcatInst(Value *dest, ArrayRef<Value *> src, size_t dim)
      : Instruction({{dest, OperandKind::kOut}}), dim_(dim) {
    for (auto s : src) {
      pushOperand({s, OperandKind::kIn});
    }
  }
  StringRef getValueName() override { return "concat"; }

  std::string getExtraDesc() override;
};

class BatchNormalizationInst : public Instruction {
  const size_t channelIdx_;
  const float epsilon_;
  const float momentum_;

public:
  BatchNormalizationInst(Value *dest, Value *src, Value *scale, Value *bias,
                         Value *mean, Value *var, size_t channelIdx,
                         float epsilon, float momentum)
      : Instruction({{dest, OperandKind::kOut},
                     {src, OperandKind::kIn},
                     {scale, OperandKind::kIn},
                     {bias, OperandKind::kIn},
                     {mean, OperandKind::kInOut},
                     {var, OperandKind::kInOut}}),
        channelIdx_(channelIdx), epsilon_(epsilon), momentum_(momentum) {}

  StringRef getValueName() override { return "batchnorm"; }

  std::string getExtraDesc() override;
};

class ArithmeticInst : public Instruction {
public:
  /// Specifies the kind of pooling done by the operator.
  enum class OpKind {
    kAdd,
    kMul,
  };

private:
  OpKind kind_;

  const char *getKindStr() {
    const char *names[] = {"add", "mul", nullptr};
    return names[(int)kind_];
  }

public:
  ArithmeticInst(Value *dest, Value *LHS, Value *RHS, OpKind kind)
      : Instruction({{dest, OperandKind::kOut},
                     {LHS, OperandKind::kIn},
                     {RHS, OperandKind::kIn}}),
        kind_(kind) {}

  StringRef getValueName() override { return "arithmetic"; }

  std::string getExtraDesc() override;
};

class StaticVariable : public Value {
public:
  enum class InitKind {
    kExtern,    // No initialization.
    kBroadcast, // Broadcast a single value to all elements.
    kXavier,    // Init the tensor with random values using the Xavier method.
  };

private:
  /// The type of the tensor to allocate.
  TypeRef Ty_;

  /// The value to use during initialization. This can be the value to splat or
  /// a parameter to specify the range of the random values.
  float val_;

  /// The initialization mode.
  InitKind mode_;

  const char *getKindStr();

public:
  StaticVariable(TypeRef Ty, InitKind mode, float val)
      : Value(), Ty_(Ty), val_(val), mode_(mode) {}
  StringRef getValueName() override { return "static"; }
  InitKind getMode() { return mode_; }
  float getVal() { return val_; }
  TypeRef getType() { return Ty_; }

  std::string getExtraDesc() override;
};

} // namespace glow

#endif // GLOW_IR_INSTRS_H