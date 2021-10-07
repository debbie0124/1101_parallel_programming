#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);    // set zero = (type: __pp_vec_float)[0., 0., 0., 0.]
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  __pp_vec_float x, result;
  __pp_vec_int y, count;
  __pp_vec_int zero = _pp_vset_int(0);
  __pp_vec_int one_int = _pp_vset_int(1);
  __pp_vec_float one_float = _pp_vset_float(1.f);
  __pp_vec_float exceed = _pp_vset_float(9.999999f);
  __pp_mask maskAll, maskIsZero, maskIsNotZero, maskCount, maskIsExceeded;
  float exceed_value = 9.999999f;
  int flag = 0;

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    // Set mask size
    maskAll = ((i + VECTOR_WIDTH) > N) ? _pp_init_ones(N - i) : _pp_init_ones();
    maskIsZero = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll);  // float x = values[i];

    // Load vector of exponents from contiguous memory addresses
    _pp_vload_int(y, exponents + i, maskAll);  // int y = exponents[i];

    // Set mask according to predicate
    _pp_veq_int(maskIsZero, y, zero, maskAll); // if (y == 0) {

    // Execute instruction using mask ("if" clause)
    // Write results back to memory with mask
    _pp_vstore_float(output + i, one_float, maskIsZero);  // output[i] = 1.f;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotZero = _pp_mask_not(maskIsZero);  // } else {
    maskIsNotZero = _pp_mask_and(maskAll, maskIsNotZero);  // deal with out of bound

    // Execute instructions ("else" clause)
    // Set value from x to result with mask
    _pp_vmove_float(result, x, maskIsNotZero);  // float result = x;

    // Initial vector of count with mask
    _pp_vsub_int(count, y, one_int, maskIsNotZero);  // int count = y - 1;

    // Set mask for doing multiplication
    _pp_vgt_int(maskCount, count, zero, maskIsNotZero);
    
    // Execution multication until all finished
    while (_pp_cntbits(maskCount)) {  // while (count > 0) {
      _pp_vmult_float(result, result, x, maskCount);  // result *= x;
      _pp_vsub_int(count, count, one_int, maskIsNotZero);  // count--; }

      // Update mask for vector of count
      _pp_vgt_int(maskCount, count, zero, maskIsNotZero);
    }

    // Set mask for exceed value
    _pp_vgt_float(maskIsExceeded, result, exceed, maskIsNotZero);  //if (result > 9.999999f) {

    // Set value for exceed value with mask
    _pp_vset_float(result, exceed_value, maskIsExceeded);  // result = 9.999999f; }

    // Write remaining results back to memory
    _pp_vstore_float(output + i, result, maskIsNotZero);  // output[i] = result; }
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{
  __pp_vec_float x;
  __pp_vec_float sum = _pp_vset_float(0.f);  // float sum = 0;

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
  }

  return 0.0;
}
