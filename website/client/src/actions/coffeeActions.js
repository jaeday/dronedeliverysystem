import axios from "axios";
export const getAllCoffees=()=>async dispatch=>{
    dispatch({type:'GET_COFFEES_REQUEST'})

    try{
        const response = await axios.get('/api/coffees/getallcoffees')
        console.log(response);
        dispatch({type:'GET_COFFEES_SUCCESS', payload: response.data})
    } catch(error){
        dispatch({type: 'GET_COFFEES_FAILED', payload: error})
    }
}