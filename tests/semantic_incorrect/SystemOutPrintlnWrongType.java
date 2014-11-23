/* Should produce 8 semantic errors. */
class A {
	public static void main(String[] args) {
		int a = 0;
		boolean b = true;
		int[] c = new int[1];
		boolean[] d = new boolean[1];
		Class2 e = new Class2();
		
		System.out.println(a);
		
		System.out.println(b);
		System.out.println(c);
		System.out.println(d);
		System.out.println(e);
		
		System.out.println(null); 
		System.out.println(true);
		System.out.println(new int[1]);
		System.out.println(new Class2());
	}
}
class Class2 {}