#pragma once

#include <set>
#include <cstddef>
#include <optional>
#include "select_container.h"

struct SingleSelectContainer : public SelectContainer {
    public:
        // NOTE: Still passes if the SingleSelectContainer are associated with different SelectOptions
        bool operator==(const SingleSelectContainer& other) {
            return getSelection() == other.getSelection();
        }

        bool operator!=(const SingleSelectContainer& other) {
            return getSelection() != other.getSelection();
        }

        friend bool operator<(const SingleSelectContainer& left, const SingleSelectContainer& right) {
            if (left.m_selection.size() == 0) {
                if (right.m_selection.size() > 0) {
                    return true;
                } else {
                    return false;
                }
            }
            if (right.m_selection.size() == 0) {
                return false;
            }

            return *left.m_selection.begin() < *right.m_selection.begin();
        }

        friend bool operator>(const SingleSelectContainer& left, const SingleSelectContainer& right) {
            if (right.m_selection.size() == 0) {
                if (left.m_selection.size() > 0) {
                    return true;
                } else {
                    return false;
                }
            }
            if (left.m_selection.size() == 0) {
                return false;
            }

            return *left.m_selection.begin() > *right.m_selection.begin();
        }

        std::optional<size_t> getSelection() const;
        // The selection set will be empty if the optional is empty or the contained index if it isn't
        void replaceSelection(const std::optional<size_t>& selection);
        // Unlike a (multi-)select, setting an index's selected either replaces or empties the entire selection
        void setSelected(size_t index, bool selected);
        bool contains(const SingleSelectContainer& other) const;
};
