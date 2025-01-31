#pragma once

#include "z5/filesystem/factory.hxx"

#ifdef WITH_S3
#include "z5/s3/factory.hxx"
#endif

#ifdef WITH_GCS
#include "z5/gcs/factory.hxx"
#endif


namespace z5 {


    template<class GROUP>
    inline std::unique_ptr<Dataset> openDataset(const handle::Group<GROUP> & root,
                                                const std::string & key) {

        // check if this is a s3 group
        #ifdef WITH_S3
        if(root.isS3()) {
            s3::handle::Dataset ds(root, key);
            return s3::openDataset(ds);
        }
        #endif
        #ifdef WITH_GCS
        if(root.isGcs()) {
            gcs::handle::Dataset ds(root, key);
            return gcs::openDataset(ds);
        }
        #endif

        filesystem::handle::Dataset ds(root, key);
        return filesystem::openDataset(ds);
    }


    template<class GROUP>
    inline std::unique_ptr<Dataset> createDataset(
        const handle::Group<GROUP> & root,
        const std::string & key,
        const DatasetMetadata & metadata
    ) {
        #ifdef WITH_S3
        if(root.isS3()) {
            s3::handle::Dataset ds(root, key);
            return s3::createDataset(ds, metadata);
        }
        #endif
        #ifdef WITH_GCS
        if(root.isGcs()) {
            gcs::handle::Dataset ds(root, key);
            return gcs::createDataset(ds, metadata);
        }
        #endif

        filesystem::handle::Dataset ds(root, key);
        return filesystem::createDataset(ds, metadata);
    }


    template<class GROUP>
    inline std::unique_ptr<Dataset> createDataset(
        const handle::Group<GROUP> & root,
        const std::string & key,
        const std::string & dtype,
        const types::ShapeType & shape,
        const types::ShapeType & chunkShape,
        const std::string & compressor="raw",
        const types::CompressionOptions & compressionOptions=types::CompressionOptions(),
        const double fillValue=0
    ) {
        DatasetMetadata metadata;
        createDatasetMetadata(dtype, shape, chunkShape, root.isZarr(),
                              compressor, compressionOptions, fillValue,
                              metadata);

        #ifdef WITH_S3
        if(root.isS3()) {
            s3::handle::Dataset ds(root, key);
            return s3::createDataset(ds, metadata);
        }
        #endif
        #ifdef WITH_GCS
        if(root.isGcs()) {
            gcs::handle::Dataset ds(root, key);
            return gcs::createDataset(ds, metadata);
        }
        #endif

        filesystem::handle::Dataset ds(root, key);
        return filesystem::createDataset(ds, metadata);
    }


    // dataset creation from json, because wrapping the CompressionOptions type
    // to python is very brittle
    template<class GROUP>
    inline std::unique_ptr<Dataset> createDataset(
        const handle::Group<GROUP> & root,
        const std::string & key,
        const std::string & dtype,
        const types::ShapeType & shape,
        const types::ShapeType & chunkShape,
        const std::string & compressor="raw",
        const nlohmann::json & compressionOptions=nlohmann::json(),
        const double fillValue=0
    ) {
        types::Compressor internalCompressor;
        try {
            internalCompressor = types::Compressors::stringToCompressor().at(compressor);
        } catch(const std::out_of_range & e) {
            throw std::runtime_error("z5::createDataset: Invalid compressor for dataset");
        }

        types::CompressionOptions cOpts;
        types::jsonToCompressionType(compressionOptions, cOpts);

        return createDataset(root, key, dtype, shape, chunkShape, compressor, cOpts, fillValue);
    }


    template<class GROUP>
    inline void createFile(const handle::File<GROUP> & file, const bool isZarr) {
        #ifdef WITH_S3
        if(file.isS3()) {
            s3::createFile(file, isZarr);
            return;
        }
        #endif
        #ifdef WITH_GCS
        if(file.isGcs()) {
            gcs::createFile(file, isZarr);
            return;
        }
        #endif
        filesystem::createFile(file, isZarr);
    }


    template<class GROUP>
    inline void createGroup(const handle::Group<GROUP> & root, const std::string & key) {
        #ifdef WITH_S3
        if(root.isS3()) {
            s3::handle::Group newGroup(root, key);
            s3::createGroup(newGroup, root.isZarr());
            return;
        }
        #endif
        #ifdef WITH_GCS
        if(root.isGcs()) {
            gcs::handle::Group newGroup(root, key);
            gcs::createGroup(newGroup, root.isZarr());
            return;
        }
        #endif
        filesystem::handle::Group newGroup(root, key);
        filesystem::createGroup(newGroup, root.isZarr());
    }


    template<class GROUP1, class GROUP2>
    inline std::string relativePath(const handle::Group<GROUP1> & g1,
                                    const GROUP2 & g2) {
        #ifdef WITH_S3
        if(g1.isS3()) {
            if(!g2.isS3()) {
                throw std::runtime_error("Can't get relative path of different backends.");
            }
            return s3::relativePath(g1, g2);
        }
        #endif
        #ifdef WITH_GCS
        if(g1.isGcs()) {
            if(!g2.isGcs()) {
                throw std::runtime_error("Can't get relative path of different backends.");
            }
            return gcs::relativePath(g1, g2);
        }
        #endif
        return filesystem::relativePath(g1, g2);
    }

}
