//
//  cluster_manager.hpp
//  blocksci
//
//  Created by Harry Kalodner on 7/6/17.
//
//

#ifndef cluster_manager_hpp
#define cluster_manager_hpp

#include "cluster.hpp"

#include <blocksci/data_access.hpp>

#include <boost/range/iterator_range.hpp>
#include <boost/range/counting_range.hpp>

#include <stdio.h>

class Cluster;

template<blocksci::ScriptType::Enum type>
struct ScriptClusterIndexFile : public blocksci::FixedSizeFileMapper<uint32_t> {
    using blocksci::FixedSizeFileMapper<uint32_t>::FixedSizeFileMapper;
};

class ClusterExpander;

struct TaggedCluster {
    Cluster cluster;
    std::vector<TaggedAddressPointer> taggedAddresses;
    
    TaggedCluster(const Cluster &cluster_, std::vector<TaggedAddressPointer> &&taggedAddresses_) : cluster(cluster_), taggedAddresses(taggedAddresses_) {}
};

class ClusterManager {
    blocksci::FixedSizeFileMapper<uint32_t> clusterOffsetFile;
    blocksci::FixedSizeFileMapper<blocksci::AddressPointer> clusterAddressesFile;
    
    using ScriptClusterIndexTuple = blocksci::internal::to_script_type<ScriptClusterIndexFile, blocksci::ScriptInfoList>::type;
    
    ScriptClusterIndexTuple scriptClusterIndexFiles;
    
public:
    ClusterManager(std::string baseDirectory);
    
    Cluster getCluster(const blocksci::AddressPointer &address) const;
    
    template<blocksci::ScriptType::Enum type>
    uint32_t getClusterNum(uint32_t addressNum) const {
        auto &file = std::get<ScriptClusterIndexFile<type>>(scriptClusterIndexFiles);
        return *file.getData(addressNum);
    }
    
    uint32_t getClusterNum(const blocksci::AddressPointer &address) const;
    
    uint32_t getClusterSize(uint32_t clusterNum) const;
    boost::iterator_range<const blocksci::AddressPointer *> getClusterAddresses(uint32_t clusterNum) const;
    
    uint32_t clusterCount() const;
    
    boost::transformed_range<ClusterExpander, const boost::iterator_range<boost::iterators::counting_iterator<unsigned int>>> getClusters() const;
    
    std::vector<uint32_t> getClusterSizes() const;
    
    std::vector<TaggedCluster> taggedClusters(const std::unordered_map<blocksci::AddressPointer, std::string> &tags);
};

class ClusterExpander
{
public:
    typedef const Cluster result_type;
    
    const ClusterManager &manager;
    ClusterExpander(const ClusterManager &manager_) : manager(manager_) {}
    
    result_type operator()(uint32_t clusterNum) const {
        return Cluster(clusterNum, manager);
    }
};

#endif /* cluster_manager_hpp */