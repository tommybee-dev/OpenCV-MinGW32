import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class HelloCV {
		static
		{
			System.load(new java.io.File(".").getAbsolutePath() + java.io.File.separator + "libopencv_java410.dll");
		}
        public static void main(String[] args){
                //System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
                
                Mat mat = Mat.eye(3, 3, CvType.CV_8UC1);
                Mat mat1 = Mat.zeros(3, 3, CvType.CV_8UC1) ;
                System.out.println("mat = " + mat.dump());
        }
}