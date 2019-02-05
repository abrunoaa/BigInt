#include "bigintCheck.h"

#define private public
#define protected public

#include "bigint.hpp"
#include "bigint.cpp"
using namespace math;

#include <iostream>
using namespace std;

vector<bigint> number;

namespace std{
  template<class T>
  ostream& operator << (ostream& out, const vector<T>& v){
    bool flag = 0;
    out << '[';
    for(auto& x : v){
      if(flag){
        out << ", ";
      }
      else{
        flag = 1;
      }
      out << x;
    }
    return out << ']';
  }
}

bigint toBigint(JNIEnv* env, const jintArray& arr){
  jint* x = env->GetIntArrayElements(arr, NULL);
  jsize n = env->GetArrayLength(arr);

  assert(x != NULL);
  assert(n > 0);

  bigint b;
  if(x[0] == 0){
    b.number_sign = 0;
    b.number.clear();
  }
  else{
    b.number_sign = x[0] < 0;
    b.number.reserve(n - 1);
    for(int i = 1; i < n; ++i){
      b.number.push_back(x[i]);
    }
    assert(b.number.back() != 0);
  }

  return b;
}

JNIEXPORT void JNICALL Java_bigintCheck_saveBigint(JNIEnv* env, jobject, jintArray arr){
  number.push_back(std::move(toBigint(env, arr)));
}

bool check(const bigint& result, const bigint& expected){
  bool wrong_sign = result.number_sign != expected.number_sign;
  bool wrong_size = result.number.size() != expected.number.size();
  bool wrong_value = 0;

  if(wrong_sign) cout << "Wrong sign!\n";
  if(wrong_size) cout << "Wrong size!\n";
  else{
    int n = result.number.size();
    for(int i = 0; i < n; ++i){
      if(result.number[i] != expected.number[i]){
        cout << "Wrong number at " << i << "!\n";
        wrong_value = 1;
        break;
      }
    }
  }

  if(!wrong_sign && !wrong_size && !wrong_value){
    return 0;
  }

  cout << "  result = " << (  result.number_sign ? '-' : '+') <<   result.number << '\n';
  cout << "expected = " << (expected.number_sign ? '-' : '+') << expected.number << '\n';
  return 1;
}

JNIEXPORT jboolean JNICALL JNICALL Java_bigintCheck_minus(JNIEnv* env, jobject, jint x, jintArray expected_arr){
  bigint expected = toBigint(env, expected_arr);
  return check(-number[x], expected);
}

JNIEXPORT jboolean JNICALL Java_bigintCheck_square(JNIEnv* env, jobject, jint x, jintArray expected_arr){
  bigint expected = toBigint(env, expected_arr);
  return check(number[x].square(), expected);
}

JNIEXPORT jboolean JNICALL Java_bigintCheck_shl(JNIEnv* env, jobject, jint x, jint y, jintArray expected_arr){
  bigint expected = toBigint(env, expected_arr);
  return check(number[x] << y, expected);
}

JNIEXPORT jboolean JNICALL Java_bigintCheck_shr(JNIEnv* env, jobject, jint x, jint y, jintArray expected_arr){
  bigint expected = toBigint(env, expected_arr);
  return check(number[x] >> y, expected);
}

#define TWO_OP_CHECK(operation)                                                            \
  bigint expected = toBigint(env, expected_arr);                                           \
  return check(operation, expected);

JNIEXPORT jboolean JNICALL Java_bigintCheck_add(JNIEnv* env, jobject, jint x, jint y, jintArray expected_arr){
  TWO_OP_CHECK(number[x] + number[y]);
}

JNIEXPORT jboolean JNICALL Java_bigintCheck_sub(JNIEnv* env, jobject, jint x, jint y, jintArray expected_arr){
  TWO_OP_CHECK(number[x] - number[y]);
}

JNIEXPORT jboolean JNICALL Java_bigintCheck_mul(JNIEnv* env, jobject, jint x, jint y, jintArray expected_arr){
  TWO_OP_CHECK(number[x] * number[y]);
}

JNIEXPORT jboolean JNICALL Java_bigintCheck_div(JNIEnv* env, jobject, jint x, jint y, jintArray expected_arr){
  TWO_OP_CHECK(number[x] / number[y]);
}

JNIEXPORT jboolean JNICALL Java_bigintCheck_rem(JNIEnv* env, jobject, jint x, jint y, jintArray expected_arr){
  TWO_OP_CHECK(number[x] % number[y]);
}
