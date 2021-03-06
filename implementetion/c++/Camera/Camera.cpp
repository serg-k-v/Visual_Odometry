#include "Camera.hpp"

void
Camera::read_property_cam(std::string path) {
    std::ifstream fin(path, std::ifstream::in);
    if (fin.is_open()) {
        Eigen::MatrixXd tmp(3,3);
        for (size_t i = 0; i < 3; i++) {
            for (size_t j = 0; j < 3; j++) {
                fin >> tmp(i,j);
            }
        }
        this->cam_pose << tmp.row(0).transpose();
        this->normal   << tmp.row(1).transpose();
        this->horizon  << tmp.row(2).transpose();
        this->vertical << this->horizon.cross(this->normal); // == x * n

    } else {
        std::cout << "File wasn't open!" << std::endl;
    }
    fin.close();
}

void
Camera::write_to_file(std::string path) {
    std::ofstream fout(path, std::ofstream::app);
    if(!fout.is_open()){
        std::cout << "data doesn't read" << '\n';
    }
    for (size_t i = 0; i < (size_t)this->features.cols(); i++) {
        fout << this->features(0, i) << ' ' << this->features(1, i) << ' ' << this->features(2, i) << '\n';
    }
    fout << '\n';

    fout.close();
}

void
Camera::get_homogen_coord() {
    size_t count_features =  this->features.cols();
    Eigen::Matrix3d transf_m;
    transf_m << horizon, vertical, normal;
    for (size_t i = 0; i < count_features; i++) {
        Eigen::Vector3d el = this->features.col(i);
        this->features.col(i) = Eigen::Vector3d(el[0]/el[2], el[2]/el[2], el[1]/el[2]);
        /*
         *    geting global position coordinates points in camera Coordinate.
         */
        
        // this->features.col(i) = transf_m*(Eigen::Vector3d(el[0]/el[2], 
        //                                      el[1]/el[2], el[2]/el[2])) + cam_pose;

    }
}

void
Camera::transform_featutes_to_local_coord() {
    // this->vertical << this->horizon.cross(this->normal); // == x * n

    // normalisation of coordinates
    this->normal   = this->normal.normalized();     // y
    this->horizon  = this->horizon.normalized();    // x
    this->vertical = this->vertical.normalized();   //z

    std::cout << "n = " << this->normal.transpose() << "\nh = "
              << this->horizon.transpose()
              << "\nv = " << this->vertical.transpose() << '\n';

    Eigen::Matrix3d transf_m;
    transf_m << this->horizon, this->vertical, this->normal;
    // transf_m << this->horizon, this->normal, this->vertical;
    // std::cout << "Transform matrix \n" << transf_m <<'\n';

    std::cout << "size = " << this->features.cols() << '\n';
    for (size_t i = 0; i < (size_t)features.cols(); i++) {
        // std::cout << "el = " << features.col(i) << '\n';
        this->features.col(i) << transf_m.inverse() * (this->features.col(i) - this->cam_pose);
    }
    this->is_local_features = true;
}

void
Camera::transform_featutes_to_global_coord(){
    // this->vertical << this->horizon.cross(this->normal); // == x * n
    if (!this->is_local_features){
        std::cout << "Already in global coordinares!" << std::endl;
        return;
    }
    // normalisation of coordinates
    // this->normal   = this->normal.normalized();     // y
    // this->horizon  = this->horizon.normalized();    // x
    // this->vertical = this->vertical.normalized();   //z

    std::cout << "n = " << this->normal.transpose() << "\nh = "
              << this->horizon.transpose()
              << "\nv = " << this->vertical.transpose() << '\n';

    Eigen::Matrix3d transf_m;
    transf_m << this->horizon, this->vertical, this->normal;
    // transf_m << this->horizon, this->normal, this->vertical;
    // std::cout << "Transform matrix \n" << transf_m <<'\n';

    std::cout << "size = " << this->features.cols() << '\n';
    for (size_t i = 0; i < (size_t)features.cols(); i++) {
        // std::cout << "el = " << features.col(i) << '\n';
        this->features.col(i) << transf_m * this->features.col(i) + this->cam_pose;
    }
    this->is_local_features = false;
}
