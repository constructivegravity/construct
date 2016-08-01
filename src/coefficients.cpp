#include <thread>

#include <common/task_pool.hpp>
#include <common/time_measurement.hpp>
#include <common/progressbar.hpp>

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/permutation.hpp>
#include <tensor/symmetrization.hpp>

#include <tensor/index_parser.hpp>

#include <language/api.hpp>
//#include <lib/tensor/tensor_database.hpp>

typedef std::pair<unsigned, unsigned>               OneCoefficient;
typedef std::pair<OneCoefficient, OneCoefficient>   Coefficient;

inline unsigned NumberOfIndices(const Coefficient& coeff) {
    return coeff.first.first + coeff.first.second + coeff.second.first + coeff.second.second;
}

std::string ToString(const Coefficient& coeff) {
    std::stringstream ss;
    ss << "(" << coeff.first.first << "," << coeff.first.second << ")" << " " <<  "(" << coeff.second.first << "," << coeff.second.second << ")";
    return ss.str();
}

Tensor GenerateTensor(const Coefficient& coeff, std::mutex& mutex, std::string& out, bool printSteps=false) {
    auto indices = Construction::Tensor::Indices::GetRomanSeries(NumberOfIndices(coeff), {1,3});

    // If no indices, return scalar
    if (indices.Size() == 0) {
        return Construction::Tensor::Tensor::One();
    }

    // Generate the tensor
    std::string previous = "Tensor(" + indices.ToCommand() + ")";
    if (printSteps) std::cerr << previous << std::endl;

    Construction::Tensor::Tensor tensors = Construction::Language::API::Arbitrary(indices);
    // Simplify the expression
    tensors.Simplify();
    tensors.RedefineVariables("e");

    // Symmetrize in the blocks
    auto block1 = Construction::Tensor::Indices::GetRomanSeries(coeff.first.first, {1,3});
    auto block2 = Construction::Tensor::Indices::GetRomanSeries(coeff.first.second, {1,3}, coeff.first.first);
    auto block3 = Construction::Tensor::Indices::GetRomanSeries(coeff.second.first, {1,3}, coeff.first.first + coeff.first.second);
    auto block4 = Construction::Tensor::Indices::GetRomanSeries(coeff.second.second, {1,3}, coeff.first.first + coeff.first.second + coeff.second.first);

    if (block1.Size() > 0) {
        previous = "Symmetrize(" + previous + "," + block1.ToCommand() + ")";
        if (printSteps) std::cerr << previous << std::endl;

        tensors = Construction::Language::API::Symmetrize(tensors, block1);

        // Simplify the expression
        tensors.Simplify();
        tensors.RedefineVariables("e");
    }
    if (block2.Size() > 0) {
        previous = "Symmetrize(" + previous + "," + block2.ToCommand() + ")";
        if (printSteps) {
            std::cerr << previous << std::endl;
        }

        tensors = Construction::Language::API::Symmetrize(tensors, block2);

        // Simplify the expression
        tensors.Simplify();
        tensors.RedefineVariables("e");
    }
    if (block3.Size() > 0) {
        previous = "Symmetrize(" + previous + "," + block3.ToCommand() + ")";
        if (printSteps) std::cerr << previous << std::endl;

        tensors = Construction::Language::API::Symmetrize(tensors, block3);

        // Simplify the expression
        tensors.Simplify();
        tensors.RedefineVariables("e");
    }
    if (block4.Size() > 0) {
        previous = "Symmetrize(" + previous + "," + block4.ToCommand() + ")";
        if (printSteps) std::cerr << previous << std::endl;

        tensors = Construction::Language::API::Symmetrize(tensors, block4);

        // Simplify the expression
        tensors.Simplify();
        tensors.RedefineVariables("e");
    }

    // Implement the exchange symmetry
    Indices newIndices = block2;
    newIndices.Append(block1);
    newIndices.Append(block4);
    newIndices.Append(block3);

    previous = "ExchangeSymmetrize(" + previous + "," + newIndices.ToCommand() + ")";
    if (printSteps) std::cerr << previous << std::endl;

    tensors = Construction::Language::API::ExchangeSymmetrize(tensors, indices, newIndices);

    // Simplify the expression
    tensors.Simplify();
    tensors.RedefineVariables("e");

    return tensors;

    // Set the output
    out = previous;

    return tensors;
}

std::vector<Coefficient> GenerateCoefficientList(int order, int maxDerivatives=3) {
    std::vector<Coefficient> result;
    for (int k=0; k<=order; k++) {
        for (int l=k; l<=order; l++) {

            if (k == 1 || l == 1) continue;

            for (int m=0; m<=maxDerivatives; m++) {
                int start = (k == l) ? m : 0;
                for (int n=start; n<=maxDerivatives; n++) {
                    Coefficient c = { { k,l }, { m,n } };

                    // if already in the list, do not add
                    // This is only true for coefficients that
                    if (l == 0 && k > 0) continue;
                    if (k == 0 && m > 0) continue;
                    if (l == 0 && n > 0) continue;

                    result.push_back(c);
                }
            }
        }
    }
    return result;
}

std::string TensorToString(const std::string& prefix, const Coefficient& coeff, const Tensor& container) {
    std::stringstream ss;

    // Print coefficient name
    ss << prefix;
    if (coeff.first.first != 0) {
        ss << "_" << coeff.first.first;
        if (coeff.second.first != 0)
            ss << "^" << coeff.second.first;

        if (coeff.first.second != 0)
            ss << "|";
    }

    if (coeff.first.second != 0) {
        ss << "_" << coeff.first.second;
        if (coeff.second.second != 0)
            ss << "^" << coeff.second.second;
    }

    ss << " : " << std::endl;

    // Print tensors
    //if (container.Size() == 0) ss << "   0" << std::endl;

    ss << container.ToString() << std::endl;

    /*int i=1;
    for (auto& tensor : container) {
        if (tensor->IsAdded())
            ss << "   " << "e_" << i << " * (" << tensor->ToString() << ") ";
        else
            ss << "   " << "e_" << i << " * " << tensor->ToString() << " ";
        if (i != container.Size()) ss << " + " << std::endl;
        i++;
    }*/

    return ss.str();
}

int main(int argc, char** argv) {

    //Construction::Tensor::TensorDatabase database;
    std::mutex mutex;
    std::mutex coutMutex;

    if (argc < 5) {

        // Load database if present
        /*std::ifstream file("tensors.db");
        if (file) {
            file.close();
            try {
                database.LoadFromFile("tensors.db");
            } catch (...) {
                database.Clear();
            }
        } else file.close();

        for (auto it = database.begin(); it != database.end(); it++) {
            std::cout << it->first;
        }
        */

        // Generate coefficient list
        auto coefficients = GenerateCoefficientList(4, 3);

        std::vector<std::thread> threads;
        Construction::Common::TaskPool pool(10);

        Construction::Common::ProgressBar progress(coefficients.size(), 100);
        progress.Start();

        for (auto &c : coefficients) {

            pool.Enqueue([&]() {
                std::string cmd;
                auto tensor = GenerateTensor(c, mutex, cmd);

                progress++;

                coutMutex.lock();
                std::cout << TensorToString("\\lambda", c, tensor) << std::endl;
                coutMutex.unlock();

                mutex.lock();
                //database[cmd] = tensor;

                //database.SaveToFile("tensors.db");
                mutex.unlock();
            });

        }

        pool.Wait();

    } else {
        std::string cmd;
        Coefficient c = { { atoi(argv[1]), atoi(argv[2]) }, { atoi(argv[3]) , atoi(argv[4]) } };

        //std::cerr << "Started " << ToString(c) << " ..." << std::endl;

        auto tensor = GenerateTensor(c, mutex, cmd, true);

        coutMutex.lock();
        std::cerr << "Finished " << ToString(c) << " ..." << std::endl;
        coutMutex.unlock();

        coutMutex.lock();
        std::cout << TensorToString("\\lambda", c, tensor) << std::endl;
        coutMutex.unlock();
    }

    // Wait for all tasks to finish
    //for (auto& thread : threads) thread.join();

    /*

    std::cout << coefficients.size() << std::endl;
    std::cout << std::endl;

    for (auto& c : coefficients) {
        std::cout << ToString(c) << std::endl;
    }*/
}
