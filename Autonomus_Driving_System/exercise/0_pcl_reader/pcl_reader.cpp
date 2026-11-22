#include <iostream>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/cloud_viewer.h>

int user_data;

/*Prende il render come parametro e renderizza in testo*/
void viewerPsycho(pcl::visualization::PCLVisualizer &viewer) {
  static unsigned count = 0;
  std::stringstream ss;
  ss << "Once per viewer loop: " << count++;
  viewer.removeShape("text", 0);
  viewer.addText(ss.str(), 200, 300, "text", 0);

  // FIXME: possible race condition here:
  user_data++;
}

int main(int argc, char **argv) {
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
  pcl::io::loadPCDFile(argv[1], *cloud); // Il primo argomento è path e il seconod parametro è il puntatpre alla poitcloud

  pcl::visualization::CloudViewer viewer("Cloud Viewer");

  // blocks until the cloud is actually rendered
  viewer.showCloud(cloud); 

  // This will get called once per visualization iteration in a separated thread
  viewer.runOnVisualizationThread(viewerPsycho);
  while (!viewer.wasStopped())
    ;
  return 0;
}
