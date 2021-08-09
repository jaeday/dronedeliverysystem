import React, { useState, useEffect } from 'react'
import { useDispatch, useSelector } from 'react-redux'
import Coffee from '../components/Coffee'
import { getAllCoffees } from '../actions/coffeeActions'

export default function Homescreen() {

    const dispatch = useDispatch()

    const coffeesstate = useSelector(state => state.getAllCoffeesReducer)

    const { coffees, error, loading } = coffeesstate
    useEffect(() => {
        dispatch(getAllCoffees())
    }, [])

    return (
        <div>
            <div className="row justify-content-center">
                {loading ? (<h1>Loading...</h1>) : error ? (<h1>Something went wrong</h1>) : (
                    coffees.map(coffee => {
                        return <div className="col-md-3 m-3"  key = {coffee._id}>
                            <div>
                                <Coffee coffee={coffee} />
                            </div>
                        </div>
                    })
                )}
            </div>
        </div>
    )
}