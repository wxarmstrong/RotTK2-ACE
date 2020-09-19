public class NES {
	
	public static void padInput(int hex) {
		System.out.print("|    0,..|");
		if ( (hex&0x08) > 0) System.out.print("U");
		else System.out.print(".");
		if ( (hex&0x04) > 0) System.out.print("D");
		else System.out.print(".");
		if ( (hex&0x02) > 0) System.out.print("L");
		else System.out.print(".");
		if ( (hex&0x01) > 0) System.out.print("R");
		else System.out.print(".");
		if ( (hex&0x10) > 0) System.out.print("S");
		else System.out.print(".");
		if ( (hex&0x20) > 0) System.out.print("s");
		else System.out.print(".");
		if ( (hex&0x40) > 0) System.out.print("B");
		else System.out.print(".");
		if ( (hex&0x80) > 0) System.out.print("A");
		else System.out.print(".");
		System.out.println("|........|");
	}
	
    public static void main(String[] args) {
        for (int i=0; i<args.length; i++){
			int num = Integer.parseInt(args[i],16);
			padInput(num);
		}
    }
}