import java.math.BigInteger;
import java.util.Random;

public class bigintCheck{
  public static void main(String[] args){
    new bigintCheck().run();
  }

  static{
    System.loadLibrary("bigint");
  }

  int t;
  int s;
  int[] shifts;
  BigInteger[] number;

  /// embededd
  native void saveBigint(int[] x);

  native boolean minus(int x, int[] expected);
  native boolean square(int x, int[] expected);

  native boolean shl(int x, int n, int[] expected);
  native boolean shr(int x, int n, int[] expected);

  native boolean add(int x, int y, int[] expected);
  native boolean sub(int x, int y, int[] expected);
  native boolean mul(int x, int y, int[] expected);
  native boolean div(int x, int y, int[] expected);
  native boolean rem(int x, int y, int[] expected);
  /// \embedded

  int[] toArray(BigInteger big){
    int[] mag = null;
    int sign = big.signum();
    if(sign == 0){
      mag = new int[1];
      mag[0] = 0;
      return mag;
    }

    if(sign < 0){
      big = big.abs();
    }

    byte[] b = big.toByteArray();
    int off = 0;
    while(off < b.length && b[off] == 0){
      ++off;
    }

    mag = new int[(b.length - off + 3) / 4 + 1];

    mag[0] = sign;
    for(int i = 0; i < b.length - off; ++i){
      mag[i / 4 + 1] |= ((b[b.length - i - 1]) << (8 * (i % 4))) & (0xff << (8 * (i % 4)));
    }

    return mag;
  }

  void buildNumbers(){
    Random rnd = new Random();

    int small = 20;
    int fixed = 20;
    int random = 50;
    int rndRange = 1200;

    int fixedShift = 32;
    int randomShift = 50;
    int rndShiftRange = 1200;

    shifts = new int[fixedShift + randomShift];
    for(int i = 0; i < fixedShift + randomShift; ++i){
      shifts[i] = 1 + (i < fixedShift ? i : rnd.nextInt(rndShiftRange));
    }

    int need = 2 * small + 1 + fixed + random;
    number = new BigInteger[need];

    t = 0;
    number[t++] = BigInteger.ZERO;
    for(int i = 1; i <= 10; ++i){
      number[t++] = BigInteger.valueOf(i);
      number[t++] = BigInteger.valueOf(-i);
    }

    for(int i = 0; i < fixed + random; ++i){
      int len = 1 + (i < fixed ? i : rnd.nextInt(rndRange));
      number[t++] = new BigInteger(32 * len, rnd);
    }

    for(int i = 0; i < t; ++i){
      saveBigint(toArray(number[i]));
    }
  }

  void perror(int i, String operation){
    System.out.printf("Error in operator %s, with number at %d\n", operation, i);
    System.out.printf("x = %d\n", number[i]);
    System.exit(0);
  }

  void perror(int i, int j, String operation){
    System.out.printf("Error in operator %s, with number at %d %d\n", operation, i, j);
    System.out.printf("x = %d\n", number[i]);
    System.out.printf("y = %d\n", number[j]);
    System.exit(0);
  }

  void run(){
    System.out.printf("Building... ");
    buildNumbers();
    System.out.printf("done\n");
    System.out.printf("Running... ");

    for(int i = 0; i < t; ++i){
      if(minus(i, toArray(number[i].negate()))) perror(i, "-");
      if(square(i, toArray(number[i].multiply(number[i])))) perror(i, "^2");

      for(int s : shifts){
        if(shl(i, s, toArray(number[i].shiftLeft(s)))) perror(i, "<< " + s);
        if(shr(i, s, toArray(number[i].shiftRight(s)))) perror(i, ">> " + s);
      }

      for(int j = 0; j < t; ++j){
        if(add(i, j, toArray(number[i].add(number[j])))) perror(i, j, "+");
        if(sub(i, j, toArray(number[i].subtract(number[j])))) perror(i, j, "-");
        if(mul(i, j, toArray(number[i].multiply(number[j])))) perror(i, j, "*");

        if(number[j].compareTo(BigInteger.ZERO) == 0) continue;
        if(div(i, j, toArray(number[i].divide(number[j])))) perror(i, j, "+");
        if(rem(i, j, toArray(number[i].remainder(number[j])))) perror(i, j, "+");
      }
    }

    System.out.printf("done\n");
  }
}
